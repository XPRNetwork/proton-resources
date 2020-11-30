#include <atom/atom.hpp>

namespace proton
{
  void atom::addplan (
    const eosio::extended_asset& price,
    const uint64_t& price_days,
    const eosio::asset& resource_credits
  ) {
    require_auth(get_self());
  }

  void atom::rmvplan (const uint64_t& plan_id) {
    require_auth(get_self());

    auto plan = require_find(plan_id, "plan not found");
  }
} // namepsace contract