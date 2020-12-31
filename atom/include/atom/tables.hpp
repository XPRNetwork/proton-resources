#pragma once

using namespace eosio;
using namespace std;

namespace proton {
  /**
   * External
   */
  struct [[eosio::table, eosio::contract("eosio.system")]] delegated_bandwidth {
    name          from;
    name          to;
    asset         net_weight;
    asset         cpu_weight;

    bool is_empty()const { return net_weight.amount == 0 && cpu_weight.amount == 0; }
    uint64_t  primary_key()const { return to.value; }

    // explicit serialization macro is not necessary, used here only to improve compilation time
    EOSLIB_SERIALIZE( delegated_bandwidth, (from)(to)(net_weight)(cpu_weight) )
  };
  typedef multi_index< "delband"_n, delegated_bandwidth > del_bandwidth_table;

  /**
   * ACCOUNT
   */
  struct [[eosio::table, eosio::contract("atom")]] Account {
    name account;
    map<extended_symbol, asset> balances;

    uint64_t primary_key() const { return account.value; };
    bool empty() const { return balances.empty(); };
  };
  typedef multi_index<"accounts"_n, Account> account_table;
  /**
   * Plans
   */
  struct PlanBase {
    asset cpu_credits;
    asset net_credits;
    uint32_t ram_bytes;
    extended_asset price;
  };
  struct [[eosio::table, eosio::contract("atom")]] Plan: public PlanBase {
    uint64_t index;
    uint64_t plan_hours;
    uint32_t max_quantity;

    string name;
    string description;
    vector<string> included;

    uint64_t primary_key() const { return index; };
  };
  typedef multi_index<"plans"_n, Plan> plan_table;

  /**
   * Subscription
   */
  struct [[eosio::table, eosio::contract("atom")]] Subscription: public PlanBase {
    name account;
    uint64_t subscription_hours;
    time_point start_time = current_time_point();

    uint64_t primary_key() const { return account.value; };
    uint64_t end_time()const { return start_time.sec_since_epoch() + (subscription_hours * SECONDS_IN_HOUR); };
  };
  typedef multi_index<"subscription"_n, Subscription,
    indexed_by<"bytime"_n, const_mem_fun<Subscription, uint64_t, &Subscription::end_time>>
  > subscription_table;
}