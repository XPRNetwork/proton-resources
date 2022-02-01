#pragma once

using namespace eosio;
using namespace std;

#define SYSTEM_CONTRACT name("eosio")
#define SYSTEM_TOKEN_CONTRACT name("eosio.token")

namespace proton
{
  static constexpr auto MAX_UINT32_T = numeric_limits<uint32_t>::max();
  static constexpr auto MAX_PROCESS  = 5;
} // namespace proton