#include <atom/atom.hpp>

namespace proton {
  void atom::ontransfer (const name& from, const name& to, const asset& quantity, const string& memo) {
    // Skip if outgoing
    if (from == get_self()) {
      return;
    }

    // Skip if deposit from system accounts
    if (from == "eosio.stake"_n || from == "eosio.ram"_n || from == "eosio"_n) {
      return;
    }

    // Validate transfer
    check(to == get_self(), "Invalid Deposit");
    check_not_paused();

    // Deposit
    name token_contract = get_first_receiver();
    check(token_contract == SYSTEM_TOKEN_CONTRACT, "only eosio.token tokens are supported");
    if (quantity.symbol == XPR_SYMBOL) {
      check(quantity.amount >= MIN_XPR_DEPOSIT, "minimum deposit is 1.0000 XPR");
    }

    // Add balance
    auto balance_to_add = extended_asset(quantity, token_contract);
    add_balance(from, balance_to_add);

    // End expired subscriptions; if one gets stuck, the admin can remove it with endsub
    process(MAX_PROCESS);
  }

  void atom::withdraw (const name& account, const extended_asset& balance) {
    require_auth(account);
    check_not_paused();
    substract_balance(account, balance);
    transfer_to(account, balance, "");
  }

  void atom::transfer_to(const name& to, const extended_asset& balance, const string& memo) {
    transfer_action t_action( balance.contract, {get_self(), "active"_n} );
    t_action.send(get_self(), to, balance.quantity, memo);
  }
}