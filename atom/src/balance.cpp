#include <atom/atom.hpp>

namespace proton {
  void atom::add_balance (const eosio::name& account, const eosio::extended_asset& delta) {
    auto acc_itr = _accounts.find(account.value);
    auto ext_sym = delta.get_extended_symbol();

    if (acc_itr == _accounts.end()) {
      _accounts.emplace(get_self(), [&](auto& a) {
        a.account = account;
        a.balances = {{ ext_sym, delta.quantity }};
      });
    } else {
      _accounts.modify(acc_itr, get_self(), [&](auto& a) {
        a.balances[ext_sym] += delta.quantity;
      });
    }
  }

  void atom::substract_balance (const eosio::name& account, const eosio::extended_asset& delta) {
    auto acc_itr = _accounts.require_find(account.value, "account does not exist");
    auto ext_sym = delta.get_extended_symbol();

    // Reduce balance
    _accounts.modify(acc_itr, get_self(), [&](auto& a) {
      a.balances[ext_sym] -= delta.quantity;
      eosio::check(a.balances[ext_sym].amount >= 0, "balance too low");

      if (a.balances[ext_sym].amount == 0) {
        a.balances.erase(ext_sym);
      }
    });

    // Remove object if empty
    if (acc_itr->balances.empty()) {
      _accounts.erase(acc_itr);
    }
  }
}