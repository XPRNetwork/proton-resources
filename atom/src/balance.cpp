#include <atom/atom.hpp>

namespace proton {
  void atom::add_balance (const eosio::name& account, const eosio::extended_asset& delta) {
    auto acc_itr = _accounts.find(account.value);
    auto ext_sym = delta.get_extended_symbol();

    // Account does not exist
    if (acc_itr == _accounts.end())
    {
      _accounts.emplace(get_self(), [&](auto& a) {
        a.account = account;
        a.balances = {{ ext_sym, delta.quantity }};
      });
    }
    // Account exists
    else
    {
      _accounts.modify(acc_itr, get_self(), [&](auto& a) {
        // Balance does not exist for token
        if (a.balances.find(ext_sym) == a.balances.end())
        {
          a.balances[ext_sym] = delta.quantity;
        }
        // Balance exists for token
        else
        {
          a.balances[ext_sym] += delta.quantity;
        }
      });
    }
  }

  void atom::substract_balance (const eosio::name& account, const eosio::extended_asset& delta) {
    // Find account
    auto acc_itr = _accounts.require_find(account.value, "account does not exist");
    auto ext_sym = delta.get_extended_symbol();
    eosio::check(delta.quantity.amount > 0, "balance to substract must be positive");

    // Reduce balance
    _accounts.modify(acc_itr, get_self(), [&](auto& a) {
      // Balance does not exist
      eosio::check(a.balances.find(ext_sym) != a.balances.end(), "user does not own this token");

      // Excess balance
      if (a.balances[ext_sym].amount > delta.quantity.amount)
      {
        a.balances[ext_sym] -= delta.quantity;
      }
      // Exact balance
      else if (a.balances[ext_sym].amount == delta.quantity.amount)
      {
        a.balances.erase(ext_sym);
      }
      // Insufficient balance
      else
      {
        eosio::check(false, "user token balance too low");
      }
    });

    // Remove account if empty
    if (acc_itr->balances.empty()) {
      _accounts.erase(acc_itr);
    }
  }
}