#pragma once

// Standard.
#include <vector>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>

// Local
#include "constants.hpp"
#include "tables.hpp"

namespace proton {
  CONTRACT atom : public eosio::contract {
  public:
    using contract::contract;

    atom( eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds )
      : contract(receiver, code, ds),
        _accounts(receiver, receiver.value),
        _plans(receiver, receiver.value),
        _terms(receiver, receiver.value) {}

    ACTION addplan    ( const Plan& plan                     );
    ACTION updateplan ( const Plan& plan                     );
    ACTION removeplan ( const uint64_t& plan_index              );
    ACTION buyplan    ( const eosio::name& account,
                        const uint64_t& plan_index,
                        const uint32_t& plan_quantity        );
    ACTION planreceipt( const uint64_t& plan_index,
                        const Term& term) {
      require_recipient(term.account);
    };

    ACTION withdraw   ( const eosio::name& account,
                        const eosio::extended_asset& balance );
    ACTION process    ( const uint64_t& max                  );

    ACTION cleanup () {
      require_auth(get_self());
      
      account_table db(get_self(), get_self().value);
      auto itr = db.end();
      while(db.begin() != itr){
        itr = db.erase(--itr);
      }

      term_table db3(get_self(), get_self().value);
      auto itr3 = db3.end();
      while(db3.begin() != itr3){
        itr3 = db3.erase(--itr3);
      }

      plan_table db2(get_self(), get_self().value);
      auto itr2 = db2.end();
      while(db2.begin() != itr2){
        itr2 = db2.erase(--itr2);
      }
    }

    // This function will be called when the contract is notified of incoming or outgoing transfer actions from any contract
    [[eosio::on_notify("*::transfer")]]
    void ontransfer   ( const eosio::name& from,
                        const eosio::name& to,
                        const eosio::asset& quantity,
                        const std::string& memo );

    void delegatebw   ( const eosio::name& from,
                        const eosio::name& receiver,
                        const eosio::asset& stake_net_quantity,
                        const eosio::asset& stake_cpu_quantity,
                        bool transfer );
    void undelegatebw ( const eosio::name& from,
                        const eosio::name& receiver,
                        const eosio::asset& unstake_net_quantity,
                        const eosio::asset& unstake_cpu_quantity );
    void buyrambytes  ( const eosio::name& payer,
                        const eosio::name& receiver,
                        uint32_t bytes );

    // Action wrappers
    using withdraw_action     = eosio::action_wrapper<"withdraw"_n,     &atom::withdraw>;
    using transfer_action     = eosio::action_wrapper<"transfer"_n,     &atom::ontransfer>;
    using delegatebw_action   = eosio::action_wrapper<"delegatebw"_n,   &atom::delegatebw>;
    using undelegatebw_action = eosio::action_wrapper<"undelegatebw"_n, &atom::undelegatebw>;
    using buyrambytes_action  = eosio::action_wrapper<"buyrambytes"_n,  &atom::buyrambytes>;
    using planreceipt_action  = eosio::action_wrapper<"planreceipt"_n,  &atom::planreceipt>;

    // Initialize tables from tables.hpp
    account_table _accounts;
    plan_table _plans;
    term_table _terms;

  private:
    void add_balance (const eosio::name& account, const eosio::extended_asset& delta);
    void substract_balance (const eosio::name& account, const eosio::extended_asset& delta);
    void end_term(const Term& term);
    void transfer_to(const eosio::name& to, const eosio::extended_asset& balance, const std::string& memo);
  };
}