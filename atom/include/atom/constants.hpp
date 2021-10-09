#pragma once

#define SYSTEM_CONTRACT eosio::name("eosio")
#define SYSTEM_TOKEN_CONTRACT eosio::name("eosio.token")
#define EOSIO_PROTON_CONTRACT eosio::name("eosio.proton")
#define FEE_ACCOUNT eosio::name("eosio.ramfee")

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
  static constexpr auto SECONDS_IN_HOUR   = SECONDS_IN_MINUTE * 60;
  static constexpr auto SECONDS_IN_DAY    = SECONDS_IN_HOUR * 24;
  static constexpr auto HOURS_IN_DAY      = 24;
  static constexpr auto MAX_TERM_MONTHS   = 3;
  static constexpr auto MAX_PROCESS       = 5;
} // namespace proton