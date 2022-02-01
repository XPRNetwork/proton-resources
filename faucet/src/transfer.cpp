#include <atom/atom.hpp>

namespace proton {
void atom::ontransfer(
  const name& from,
  const name& to,
  const asset& quantity,
  const string& memo
) {
  // Skip if outgoing
  if (from == get_self()) {
    return;
  }

  // Validate transfer
  check(to == get_self(), "invalid to account");

  // Deposit
  name token_contract = get_first_receiver();
  auto token = extended_asset(quantity, token_contract);

  // Skip if deposit from system accounts
  if (from == "eosio"_n || from == "eosio.stake"_n || from == "eosio.ram"_n) {
    return;
  }

  // Split memo
  const vector<string> split_memo = split(memo, ",");

  if (split_memo.size() == 2 && split_memo[0] == "donate") {
    const uint64_t programId = stoull(split_memo[1]);
    donate(programId, from, token);
  } else {
    check(false, "Wrong memo format. Memo must be 'donate,programId'");
  }
}
} // namespace proton