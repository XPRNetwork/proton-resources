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

using namespace eosio;
using namespace std;

namespace proton {
  CONTRACT atom : public contract {
  public:
    using contract::contract;

    atom( name receiver, name code, datastream<const char*> ds )
      : contract(receiver, code, ds),
        _accounts(receiver, receiver.value),
        _plans(receiver, receiver.value),
        _subscriptions(receiver, receiver.value) {}

    ACTION addplan    ( const Plan& plan                     );
    ACTION updateplan ( const Plan& plan                     );
    ACTION removeplan ( const uint64_t& plan_index              );
    ACTION buyplan    ( const name& account,
                        const uint64_t& plan_index,
                        const uint32_t& plan_quantity        );
    ACTION planreceipt( const uint64_t& plan_index,
                        const Subscription& subscription) {
      require_auth(get_self());
      require_recipient(subscription.account);
    };

    ACTION withdraw   ( const name& account,
                        const extended_asset& balance );
    ACTION process    ( const uint64_t& max                 );

    ACTION cleanup () {
      require_auth(get_self());
      
      account_table db(get_self(), get_self().value);
      auto itr = db.end();
      while(db.begin() != itr){
        itr = db.erase(--itr);
      }

      subscription_table db3(get_self(), get_self().value);
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
    void ontransfer   ( const name& from,
                        const name& to,
                        const asset& quantity,
                        const string& memo );

    void delegatebw   ( const name& from,
                        const name& receiver,
                        const asset& stake_net_quantity,
                        const asset& stake_cpu_quantity,
                        bool transfer );
    void undelegatebw ( const name& from,
                        const name& receiver,
                        const asset& unstake_net_quantity,
                        const asset& unstake_cpu_quantity );
    void buyrambytes  ( const name& payer,
                        const name& receiver,
                        uint32_t bytes );

    // Action wrappers
    using withdraw_action     = action_wrapper<"withdraw"_n,     &atom::withdraw>;
    using transfer_action     = action_wrapper<"transfer"_n,     &atom::ontransfer>;
    using delegatebw_action   = action_wrapper<"delegatebw"_n,   &atom::delegatebw>;
    using undelegatebw_action = action_wrapper<"undelegatebw"_n, &atom::undelegatebw>;
    using buyrambytes_action  = action_wrapper<"buyrambytes"_n,  &atom::buyrambytes>;
    using planreceipt_action  = action_wrapper<"planreceipt"_n,  &atom::planreceipt>;

    // Initialize tables from tables.hpp
    account_table _accounts;
    plan_table _plans;
    subscription_table _subscriptions;

  private:
    void add_balance (const name& account, const extended_asset& delta);
    void substract_balance (const name& account, const extended_asset& delta);
    void end_subscription(const Subscription& subscription);
    void transfer_to(const name& to, const extended_asset& balance, const string& memo);
  };
}