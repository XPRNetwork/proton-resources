#include <atom/atom.hpp>

namespace proton
{
  void atom::setplan (const Plan& plan) {
    require_auth(get_self());

    auto plan_itr = _plans.find(plan.index);

    if (plan_itr == _plans.end()) {
      _plans.emplace(get_self(), [&](auto& p) {
        p = plan;
      });
    } else {
      _plans.modify(plan_itr, get_self(), [&](auto& p) {
        p = plan;
      });
    }
  }

  void atom::removeplan (const uint64_t& plan_id) {
    require_auth(get_self());

    auto plan = _plans.require_find(plan_id, "plan not found");
    _plans.erase(plan);
  }

  void atom::buyplan (
    const eosio::name& account,
    const uint64_t& plan_id
  ) {
    require_auth(account);

    // Find Plan
    auto plan_itr = _plans.require_find(plan_id, "plan not found");

    // Find account
    auto acc_itr = _accounts.require_find(account.value, "account not found");

    // Reduce balance
    substract_balance(account, plan_itr->price);

    // Save plan term
    auto term_itr = _terms.find(account.value);
    eosio::check(term_itr == _terms.end(), "a term already exists for this account");

    _terms.emplace(get_self(), [&](auto& t) {
      t.account     = account;
      t.cpu_credits = plan_itr->cpu_credits;
      t.net_credits = plan_itr->net_credits;
      t.ram_bytes   = plan_itr->ram_bytes;
      t.end_time    = eosio::current_time_point().sec_since_epoch() + (SECONDS_IN_DAY * plan_itr->days);
    });

    // Delegate CPU, NET and RAM
    // TODO

    // Process action to unstake CPU/NET
    // TODO
  }
} // namepsace contract