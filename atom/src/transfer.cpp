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
    bool valid_contract = get_first_receiver() == TOKEN_CONTRACT;
    bool valid_to = to == get_self();
    eosio::check(valid_symbol && valid_contract && valid_to, "Invalid Deposit");
  }
}