#include <atom/atom.hpp>

namespace proton
{
  void atom::hi (const eosio::name& account) {
    eosio::print(account);
  }
} // namepsace contract