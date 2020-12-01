#pragma once

// Standard.
#include <optional>
#include <vector>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>

// Local
#include "constants.hpp"

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
  typedef eosio::multi_index<"account"_n, Account> account_table;

  /**
   * Plans
   */
  struct ResourceBase {
    eosio::asset cpu_credits;
    eosio::asset net_credits;
    uint64_t ram_bytes;
  };
  struct [[eosio::table, eosio::contract("atom")]] Plan: public ResourceBase {
    uint64_t index;
    eosio::extended_asset price;
    uint64_t days;

    uint64_t primary_key() const { return index; };
  };
  typedef eosio::multi_index<"plan"_n, Plan> plan_table;

  /**
   * TERM
   */
  struct [[eosio::table, eosio::contract("atom")]] Term: public ResourceBase {
    eosio::name account;
    uint64_t end_time;

    uint64_t primary_key() const { return account.value; };
  };
  typedef eosio::multi_index<"term"_n, Term> term_table;
}