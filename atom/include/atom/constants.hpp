#pragma once

#define SYSTEM_CONTRACT eosio::name("eosio")
#define FEE_ACCOUNT eosio::name("a.resources")

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
  static constexpr auto SECONDS_IN_HOUR   = SECONDS_IN_MINUTE * 60;
  static constexpr auto SECONDS_IN_DAY    = SECONDS_IN_HOUR * 24;
  static constexpr auto MAX_TERM_MONTHS   = 3;
  static constexpr auto MAX_PROCESS       = 5;

  struct [[eosio::table, eosio::contract("eosio.system")]] delegated_bandwidth {
    eosio::name          from;
    eosio::name          to;
    eosio::asset         net_weight;
    eosio::asset         cpu_weight;

    bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0; }
    uint64_t  primary_key()const { return to.value; }

    // explicit serialization macro is not necessary, used here only to improve compilation time
    EOSLIB_SERIALIZE( delegated_bandwidth, (from)(to)(net_weight)(cpu_weight) )
  };
  typedef eosio::multi_index< "delband"_n, delegated_bandwidth > del_bandwidth_table;
} // namespace proton