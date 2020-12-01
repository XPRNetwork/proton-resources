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
#include "tables.hpp"
#include "constants.hpp"

namespace proton {
  CONTRACT atom : public eosio::contract {
  public:
    using contract::contract;

    atom( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds )
      : contract(receiver, code, ds),
        _accounts(receiver, receiver.value),
        _plans(receiver, receiver.value),
        _terms(receiver, receiver.value) {}

    ACTION setplan    ( const Plan& plan                     );
    ACTION removeplan ( const uint64_t& plan_id              );
    ACTION buyplan    ( const eosio::name& account,
                        const uint64_t& plan_id              );
    ACTION withdraw   ( const eosio::name& account,
                        const eosio::extended_asset& balance );

    // This function will be called when the contract is notified of incoming or outgoing transfer actions from any contract
    [[eosio::on_notify("*::transfer")]]
    void transfer( const eosio::name& from,
                   const eosio::name& to,
                   const eosio::asset& quantity,
                   const std::string& memo );

    // Action wrappers
    using withdraw_action = eosio::action_wrapper<"withdraw"_n, &atom::withdraw>;
    using transfer_action = eosio::action_wrapper<"transfer"_n, &atom::transfer>;

    // Initialize tables from tables.hpp
    account_table _accounts;
    plan_table _plans;
    term_table _terms;

  private:
    void add_balance (const eosio::name& account, const eosio::extended_asset& delta);
    void substract_balance (const eosio::name& account, const eosio::extended_asset& delta);

    // Private functions (not in ABI)
  };
}