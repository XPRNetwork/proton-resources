#include <atom/atom.hpp>

namespace proton {
  void atom::ontransfer (const eosio::name& from, const eosio::name& to, const eosio::asset& quantity, const std::string& memo) {
    // Process action
    process(10);

    // Skip if outgoing
    if (from == get_self()) {
      return;
    }

    // Skip if deposit memo
    if (memo == "deposit") {
      return;
    }

    // Skip if deposit from system accounts
    if (from == "eosio.stake"_n || from == "eosio.ram"_n || from == "eosio"_n) {
      return;
    }

    // Validate transfer
    eosio::check(to == get_self(), "Invalid Deposit");

    // Deposit
    eosio::name token_contract = get_first_receiver();
    auto balance_to_add = eosio::extended_asset(quantity, token_contract);
    add_balance(from, balance_to_add);
  }

  void atom::withdraw (const eosio::name& account, const eosio::extended_asset& balance) {
    require_auth(account);
    substract_balance(account, balance);
    transfer_to(account, balance, "");
  }

  void atom::transfer_to(const eosio::name& to, const eosio::extended_asset& balance, const std::string& memo) {
    transfer_action t_action( balance.contract, {get_self(), "active"_n} );
    t_action.send(get_self(), to, balance.quantity, memo);
  }
}