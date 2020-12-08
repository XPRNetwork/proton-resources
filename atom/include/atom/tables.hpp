#pragma once

namespace proton {
  /**
   * ACCOUNT
   */
  struct [[eosio::table, eosio::contract("atom")]] Account {
    eosio::name account;
    std::map<eosio::extended_symbol, eosio::asset> balances;

    uint64_t primary_key() const { return account.value; };
    bool empty() const { return balances.empty(); };
  };
  typedef eosio::multi_index<"accounts"_n, Account> account_table;

  /**
   * Plans
   */
  struct PlanBase {
    eosio::asset cpu_credits;
    eosio::asset net_credits;
    uint32_t ram_bytes;
    eosio::extended_asset price;
  };
  struct [[eosio::table, eosio::contract("atom")]] Plan: public PlanBase {
    uint64_t index;
    uint64_t plan_hours;
    uint32_t max_quantity;

    std::string name;
    std::string description;
    std::vector<std::string> included;

    uint64_t primary_key() const { return index; };
  };
  typedef eosio::multi_index<"plans"_n, Plan> plan_table;

  /**
   * Subscription
   */
  struct [[eosio::table, eosio::contract("atom")]] Subscription: public PlanBase {
    eosio::name account;
    uint64_t subscription_hours;
    eosio::time_point start_time = eosio::current_time_point();

    uint64_t primary_key() const { return account.value; };
    uint64_t end_time()const { return start_time.sec_since_epoch() + (subscription_hours * SECONDS_IN_HOUR); };
  };
  typedef eosio::multi_index<"subscription"_n, Subscription,
    eosio::indexed_by<"bytime"_n, eosio::const_mem_fun<Subscription, uint64_t, &Subscription::end_time>>
  > subscription_table;
}