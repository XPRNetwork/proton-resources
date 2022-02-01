#pragma once

using namespace eosio;
using namespace std;

namespace proton {
  struct [[eosio::table, eosio::contract("atom")]] Program {
    uint64_t index;
    name creator;
    extended_asset savedToken;
    extended_asset claimToken;
    uint32_t duration;

    uint64_t  primary_key()const { return index; }
  };
  typedef multi_index<"programs"_n, Program> programs_table;

  struct [[eosio::table, eosio::contract("atom")]] Account {
    name account;
    uint32_t nextExpiry;
    std::map<uint64_t, uint32_t> expiryByProgram;

    uint64_t primary_key() const { return account.value; };
    uint64_t by_next_expiry() const { return -nextExpiry; };
  };
  typedef multi_index<"accounts"_n, Account,
    eosio::indexed_by< "bynextexpiry"_n, eosio::const_mem_fun<Account, uint64_t, &Account::by_next_expiry>>
  > accounts_table;
}