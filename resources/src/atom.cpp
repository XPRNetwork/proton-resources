#include <atom/atom.hpp>

namespace proton
{
  void atom::addplan (const Plan& plan) {
    require_auth(get_self());
    check(_plans.find(plan.index) == _plans.end(), "plan id " + to_string(plan.index) + " already exists");
    _plans.emplace(get_self(), [&](auto& p) {
      p = plan;
    });
  }

  void atom::updateplan (const Plan& plan) {
    require_auth(get_self());
    auto plan_itr = _plans.require_find(plan.index, "plan does not exist");
    _plans.modify(plan_itr, get_self(), [&](auto& p) {
      p = plan;
    });
  }

  void atom::removeplan (const uint64_t& plan_index) {
    require_auth(get_self());

    auto plan = _plans.require_find(plan_index, string("plan id : " + to_string(plan_index) + " not found").c_str());
    _plans.erase(plan);
  }

  void atom::buyplan (
    const name& account,
    const uint64_t& plan_index,
    const uint32_t& plan_quantity
  ) {
    // Require auth
    require_auth(account);
    check_not_paused();

    // Verification
    check(plan_quantity > 0, "plan_quantity must be positive");

    // Find Plan
    auto plan_itr = _plans.require_find(plan_index, "plan not found");
    check(plan_quantity <= plan_itr->max_quantity, "too many plans. Maximum allowed is " + to_string(plan_itr->max_quantity) + " but you entered " + to_string(plan_quantity));

    // Find account
    auto acc_itr = _accounts.require_find(account.value, string("account " + account.to_string() + " not found").c_str());

    // Price and duration of the new subscription
    auto adjusted_price = plan_itr->price;
    adjusted_price.quantity.amount *= plan_quantity;
    uint64_t paid_hours = plan_itr->plan_hours * plan_quantity;

    // Save plan subscription
    auto term_itr = _subscriptions.find(account.value);

    // Process subscription upgrade if exists, carrying over unused value as extra hours
    int64_t carried_value = 0;
    uint64_t carried_hours = 0;
    if (term_itr != _subscriptions.end()) {
      carried_value = remaining_value(*term_itr, adjusted_price);
      if (carried_value > 0 && adjusted_price.quantity.amount > 0) {
        carried_hours = (uint64_t)((uint128_t)carried_value * paid_hours / adjusted_price.quantity.amount);
      }

      // Process subscription
      end_subscription(*term_itr);

      // Delete subscription
      _subscriptions.erase(term_itr);
    }

    // Reduce balance
    substract_balance(account, adjusted_price);

    // Send balance to fee collector
    if (is_account(FEE_ACCOUNT)) {
      transfer_to(FEE_ACCOUNT, adjusted_price, account.to_string());
    }

    // New Subscription
    _subscriptions.emplace(get_self(), [&](auto& t) {
      t.account     = account;
      t.cpu_credits = plan_itr->cpu_credits;
      t.net_credits = plan_itr->net_credits;
      t.ram_bytes   = 0;
      t.price       = adjusted_price;
      t.price.quantity.amount += carried_value;
      t.subscription_hours  = paid_hours + carried_hours;

      // Plan receipt
      planreceipt_action pr_action( get_self(), {get_self(), "active"_n} );
      pr_action.send(plan_itr->index, t);
    });
    
    // Delegate CPU, NET
    if (plan_itr->cpu_credits.amount > 0 || plan_itr->net_credits.amount > 0) {
      delegatebw_action db_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
      db_action.send(get_self(), account, plan_itr->net_credits, plan_itr->cpu_credits, false);
    }
  }

  void atom::process (const uint64_t& max) {
    if (_subscriptions.begin() != _subscriptions.end()) {
      auto idx = _subscriptions.get_index<"bytime"_n>();
      auto itr = idx.begin();
      auto oitr = itr;

      for (uint16_t i = 0; i < max; ++i) {
        itr = oitr;
        if (itr == idx.end() || current_time_point().sec_since_epoch() < itr->end_time()) break;
        end_subscription(*itr);
        oitr = next(itr);
        idx.erase(itr);
      }
    }
  }

  void atom::setpaused (const bool& paused) {
    require_auth(get_self());

    auto config = _config.get_or_default();
    check(config.paused != paused, paused ? "contract is already paused" : "contract is not paused");
    config.paused = paused;
    _config.set(config, get_self());
  }

  void atom::check_not_paused () {
    check(!_config.get_or_default().paused, "contract is paused");
  }

  void atom::endsub (const name& account, const bool& skip_undelegate) {
    require_auth(get_self());

    auto sub_itr = _subscriptions.require_find(account.value, "subscription not found");
    if (!skip_undelegate) {
      end_subscription(*sub_itr);
    }
    _subscriptions.erase(sub_itr);
  }

  int64_t atom::remaining_value (const Subscription& subscription, const extended_asset& price) {
    uint64_t now = current_time_point().sec_since_epoch();
    uint64_t end = subscription.end_time();
    if (now >= end || subscription.subscription_hours == 0) {
      return 0;
    }

    check(subscription.price.get_extended_symbol() == price.get_extended_symbol(),
          "cannot carry over a subscription paid in a different token");

    uint128_t total_secs = (uint128_t)subscription.subscription_hours * SECONDS_IN_HOUR;
    return (int64_t)((uint128_t)subscription.price.quantity.amount * (end - now) / total_secs);
  }

  void atom::end_subscription(const Subscription& subscription) {
    // Unstake all amounts from an account
    auto delband = del_bandwidth_table(SYSTEM_CONTRACT, get_self().value);
    auto staked = delband.find(subscription.account.value);
    if (staked != delband.end()) {
      undelegatebw_action udb_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
      udb_action.send(get_self(), subscription.account, staked->net_weight, staked->cpu_weight);
    }
  }

  void atom::newaccount (
    const name&      creator,
    const name&      name,
    const authority& owner,
    const authority& active
  ) {
    require_auth(creator);
    check_not_paused();

    substract_balance(creator, eosio::extended_asset(eosio::asset(500000, eosio::symbol("XPR", 4)), SYSTEM_TOKEN_CONTRACT));

    newaccount_action new_acct_action( SYSTEM_CONTRACT, {name.suffix(), "active"_n} );
    new_acct_action.send(get_self(), name, owner, active);

    buyrambytes_action buyram_bytes_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
    buyram_bytes_action.send(get_self(), name, 12000);
    
    newaccres_action newacc_res_action( EOSIO_PROTON_CONTRACT, {get_self(), "active"_n} );
    newacc_res_action.send(name);
  }
} // namepsace contract