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
  CONTRACT atom : public eosio::contract {
  public:
    using contract::contract;

    atom( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds )
      : contract(receiver, code, ds),
        _accounts(receiver, receiver.value) {}

    ACTION hi ( const eosio::name& account );

    // This functions will be called when the contract is notified of
    // incoming or outgoing transfer actions from the eosio.token contract
    [[eosio::on_notify("eosio.token::transfer")]]
    void transfer( const eosio::name& from,
                   const eosio::name& to,
                   const eosio::asset& quantity,
                   const std::string& memo );

    // Action wrappers
    using hi_action = eosio::action_wrapper<"hi"_n, &atom::hi>;

    // Define table
    TABLE Account {
      uint64_t index;
      uint64_t balance;
      eosio::name account;

      uint64_t primary_key() const { return index; };
      uint64_t by_balance() const { return balance; };
    };

    typedef eosio::multi_index<"account"_n, Account,
      eosio::indexed_by<eosio::name("bybalance"), eosio::const_mem_fun<Account, uint64_t, &Account::by_balance>>
    > account_table;

    // Initialize table
    account_table _accounts;

  private:
    // Private functions (not in ABI)
  };
}