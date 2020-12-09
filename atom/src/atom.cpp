#include <atom/atom.hpp>

namespace proton
{
  void atom::addplan (const Plan& plan) {
    require_auth(get_self());
    auto plan_itr = _plans.find(plan.index);
    eosio::check(plan_itr == _plans.end(), "plan already exists");
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

    auto plan = _plans.require_find(plan_index, "plan not found");
    _plans.erase(plan);
  }

  void atom::buyplan (
    const eosio::name& account,
    const uint64_t& plan_index,
    const uint32_t& plan_quantity
  ) {
    // Require auth
    require_auth(account);

    // Find Plan
    auto plan_itr = _plans.require_find(plan_index, "plan not found");
    eosio::check(plan_quantity <= plan_itr->max_quantity, "too many plans");

    // Find account
    auto acc_itr = _accounts.require_find(account.value, "account not found");

    // Save plan subscription
    auto term_itr = _subscriptions.find(account.value);

    // Process subscription upgrade if exists
    if (term_itr != _subscriptions.end()) {
      // Refund if hours left
      float hours_left = (term_itr->end_time() - eosio::current_time_point().sec_since_epoch()) / (float) SECONDS_IN_HOUR;
      if (hours_left > 0) {
        auto refund_price = term_itr->price;
        refund_price.quantity.amount = (uint64_t)((hours_left / (float)term_itr->subscription_hours) * (float)refund_price.quantity.amount);
        add_balance(account, refund_price);
      }

      // Process subscription
      end_subscription(*term_itr);

      // Delete subscription
      _subscriptions.erase(term_itr);
    }

    // Reduce balance
    auto adjusted_price = plan_itr->price;
    adjusted_price.quantity.amount *= plan_quantity;
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
      t.ram_bytes   = plan_itr->ram_bytes;
      t.price       = adjusted_price;
      t.subscription_hours  = plan_itr->plan_hours * plan_quantity;

      // Plan receipt
      planreceipt_action pr_action( get_self(), {get_self(), "active"_n} );
      pr_action.send(plan_itr->index, t);
    });
    
    // Delegate CPU, NET
    if (plan_itr->cpu_credits.amount > 0 || plan_itr->net_credits.amount > 0) {
      delegatebw_action db_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
      db_action.send(get_self(), account, plan_itr->net_credits, plan_itr->cpu_credits, false);
    }

    // Delegate RAM
    if (plan_itr->ram_bytes > 0) {
      buyrambytes_action brb_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
      brb_action.send(get_self(), account, plan_itr->ram_bytes);
    }
  }

  void atom::process (const uint64_t& max) {
    if (_subscriptions.begin() != _subscriptions.end()) {
      auto idx = _subscriptions.get_index<"bytime"_n>();
      auto itr = idx.begin();
      auto oitr = itr;

      // itr++;
      // eosio::check(false, "Account: " + itr->account.to_string() + 
      //                     " A: " + std::to_string(itr == idx.end()) + 
      //                     " B: " + std::to_string(eosio::current_time_point().sec_since_epoch() < itr->end_time()) +
      //                     " Start Time: " + std::to_string(itr->start_time.sec_since_epoch()) +
      //                     " End Time: " + std::to_string(itr->end_time()) +
      //                     " Current Time: " + std::to_string(eosio::current_time_point().sec_since_epoch())
      //             );

      for (uint16_t i = 0; i < max; ++i) {
        itr = oitr;
        if (itr == idx.end() || eosio::current_time_point().sec_since_epoch() < itr->end_time()) break;
        end_subscription(*itr);
        oitr = std::next(itr);
        idx.erase(itr);
      }
    }
  }

  void atom::end_subscription(const Subscription& subscription) {
    // Unstake all amounts from an account
    if (subscription.cpu_credits.amount > 0 || subscription.net_credits.amount > 0) {
      auto delband = del_bandwidth_table(SYSTEM_CONTRACT, get_self().value);
      auto staked = delband.require_find(subscription.account.value);

      undelegatebw_action udb_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
      udb_action.send(get_self(), subscription.account, staked->net_weight, staked->cpu_weight);
    }
  }
} // namepsace contract