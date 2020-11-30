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
    std::map<eosio::name, eosio::asset> balances;

    uint64_t primary_key() const { return account.value; };
  };
  typedef eosio::multi_index<"account"_n, Account> account_table;

  /**
   * RESOURCE
   */
  struct [[eosio::table, eosio::contract("atom")]] Plan {
    uint64_t index;
    eosio::extended_asset price;
    uint64_t price_days;
    eosio::asset resource_credits;

    uint64_t primary_key() const { return index; };
  };
  typedef eosio::multi_index<"plan"_n, Plan> plan_table;
}