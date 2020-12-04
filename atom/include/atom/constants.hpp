#pragma once

#define SYSTEM_CONTRACT eosio::name("eosio")
#define FEE_ACCOUNT eosio::name("a.resources")

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
  static constexpr auto SECONDS_IN_HOUR   = SECONDS_IN_MINUTE * 60;
  static constexpr auto SECONDS_IN_DAY    = SECONDS_IN_HOUR * 24;
  static constexpr auto MAX_TERM_MONTHS   = 3;
} // namespace proton