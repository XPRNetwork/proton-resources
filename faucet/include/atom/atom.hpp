#pragma once

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
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
        _programs(receiver, receiver.value),
        _accounts(receiver, receiver.value) {}

    ACTION insert (
      const name& creator,
      const extended_asset& claimToken,
      const uint32_t& duration
    );

    ACTION update (
      const uint64_t& programId,
      const extended_asset& claimToken,
      const uint32_t& duration
    );

    ACTION del (
      const uint64_t& programId,
      const name& account
    );

    ACTION claim (
      const uint64_t& programId,
      const name& account
    );

    ACTION process (
      const uint8_t& maxProcess
    );

    // This function will be called when the contract is notified of incoming or outgoing transfer actions from any contract
    [[eosio::on_notify("*::transfer")]]
    void ontransfer (
      const name& from,
      const name& to,
      const asset& quantity,
      const string& memo
    );

    // Action wrappers
    using insert_action = action_wrapper<"insert"_n, &atom::insert>;
    using update_action = action_wrapper<"update"_n, &atom::update>;
    using delete_action = action_wrapper<"del"_n, &atom::del>;
    using claim_action = action_wrapper<"claim"_n, &atom::claim>;
    using transfer_action = action_wrapper<"transfer"_n, &atom::ontransfer>;

    // Initialize tables from tables.hpp
    programs_table _programs;
    accounts_table _accounts;

  private:
    void donate (
      const uint64_t& programId,
      const name& account,
      const extended_asset& token
    );

    static inline vector<string> split(const string& s, const string& delim)
    {
      std::vector<std::string> res;
      auto start = 0U;
      auto end = s.find(delim);

      while (end != std::string::npos)
      {
        res.push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
      }
      res.push_back(s.substr(start, end));

      return res;
    };
  };
}