#include <atom/atom.hpp>

namespace proton {
  void atom::transfer (const eosio::name& from, const eosio::name& to, const eosio::asset& quantity, const std::string& memo) {
    // Skip if outgoing
    if (from == get_self()) {
      return;
    }

    // Skip if deposit from system accounts
    if (from == "eosio.stake"_n || from == "eosio.ram"_n || from == "eosio"_n) {
      return;
    }

    // Validate transfer
    bool valid_symbol = quantity.symbol == TOKEN_SYMBOL;
    bool valid_to = to == get_self();
    eosio::check(valid_symbol && valid_to, "Invalid Deposit");

    // Get token contract
    eosio::name token_contract = get_first_receiver();

    // Do something
    deposit(token_contract, from, quantity);
  }

  void atom::deposit (const eosio::name& contract, const eosio::name& account, const eosio::asset& quantity) {
    auto acc_itr = _accounts.find(account.value);
    
    if (acc_itr == _accounts.end()) {
      _accounts.emplace(get_self(), [&](auto& a) {
        a.account = account;
        // a.balances = { }
      });
    } else {
      _accounts.modify(acc_itr, get_self(), [&](auto& a) {
        a.balances[contract] += quantity;
      });
    }
  }

  void atom::withdraw (const eosio::name& contract, const eosio::name& account, const eosio::asset& quantity) {
    require_auth(account);

    auto acc_itr = _accounts.require_find(account.value, "account does not exist");
    
    _accounts.modify(acc_itr, get_self(), [&](auto& a) {
      a.balances[contract] -= quantity;

      if (a.balances[contract].amount == 0) {
        a.balances.erase(contract);
      }
    });

    // Remove object if no balances
    if (acc_itr->balances.size() == 0) {
      _accounts.erase(acc_itr);
    }
  }
}