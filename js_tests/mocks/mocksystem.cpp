// Minimal stand-in for the eosio and eosio.proton system contracts used by atom.
// Deployed to both accounts in tests. Stake bookkeeping mirrors eosio.system's delband
// table layout so atom can read it; no tokens are moved.

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <atom/authority.hpp>

using namespace eosio;

CONTRACT mocksystem : public contract {
public:
  using contract::contract;

  struct [[eosio::table]] delegated_bandwidth {
    name  from;
    name  to;
    asset net_weight;
    asset cpu_weight;

    uint64_t primary_key() const { return to.value; }
  };
  typedef multi_index<"delband"_n, delegated_bandwidth> del_bandwidth_table;

  struct [[eosio::table]] rambuy {
    uint64_t id;
    name     payer;
    name     receiver;
    uint32_t bytes;

    uint64_t primary_key() const { return id; }
  };
  typedef multi_index<"rambuys"_n, rambuy> rambuy_table;

  struct [[eosio::table]] created_account {
    name account;
    name creator;

    uint64_t primary_key() const { return account.value; }
  };
  typedef multi_index<"newaccts"_n, created_account> created_account_table;

  struct [[eosio::table]] resourced_account {
    name account;

    uint64_t primary_key() const { return account.value; }
  };
  typedef multi_index<"newaccres"_n, resourced_account> resourced_account_table;

  ACTION delegatebw (const name& from, const name& receiver,
                     const asset& stake_net_quantity, const asset& stake_cpu_quantity, const bool& transfer) {
    require_auth(from);
    check(stake_net_quantity.amount >= 0 && stake_cpu_quantity.amount >= 0, "must stake a positive amount");
    check(stake_net_quantity.amount + stake_cpu_quantity.amount > 0, "must stake a positive amount");

    del_bandwidth_table del(get_self(), from.value);
    auto itr = del.find(receiver.value);
    if (itr == del.end()) {
      del.emplace(from, [&](auto& d) {
        d.from = from;
        d.to = receiver;
        d.net_weight = stake_net_quantity;
        d.cpu_weight = stake_cpu_quantity;
      });
    } else {
      del.modify(itr, same_payer, [&](auto& d) {
        d.net_weight += stake_net_quantity;
        d.cpu_weight += stake_cpu_quantity;
      });
    }
  }

  ACTION undelegatebw (const name& from, const name& receiver,
                       const asset& unstake_net_quantity, const asset& unstake_cpu_quantity) {
    require_auth(from);

    del_bandwidth_table del(get_self(), from.value);
    const auto& d = del.get(receiver.value, "no delegated bandwidth");
    check(d.net_weight.amount >= unstake_net_quantity.amount, "insufficient staked net bandwidth");
    check(d.cpu_weight.amount >= unstake_cpu_quantity.amount, "insufficient staked cpu bandwidth");

    del.modify(d, same_payer, [&](auto& row) {
      row.net_weight -= unstake_net_quantity;
      row.cpu_weight -= unstake_cpu_quantity;
    });
    if (d.net_weight.amount == 0 && d.cpu_weight.amount == 0) {
      del.erase(d);
    }
  }

  ACTION buyrambytes (const name& payer, const name& receiver, const uint32_t& bytes) {
    require_auth(payer);
    rambuy_table buys(get_self(), get_self().value);
    buys.emplace(payer, [&](auto& b) {
      b.id = buys.available_primary_key();
      b.payer = payer;
      b.receiver = receiver;
      b.bytes = bytes;
    });
  }

  ACTION newaccount (const name& creator, const name& name,
                     ignore<authority> owner, ignore<authority> active) {
    require_auth(creator);
    created_account_table accts(get_self(), get_self().value);
    accts.emplace(creator, [&](auto& a) {
      a.account = name;
      a.creator = creator;
    });
  }

  ACTION newaccres (const name& account) {
    resourced_account_table accts(get_self(), get_self().value);
    accts.emplace(get_self(), [&](auto& a) {
      a.account = account;
    });
  }
};
