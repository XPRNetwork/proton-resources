#include <atom/atom.hpp>

namespace proton
{
  void atom::insert (
    const name& creator,
    const extended_asset& claimToken,
    const uint32_t& duration
  ) {
    // Authenticate
    require_auth(get_self());

    // Params validation
    check(claimToken.quantity.is_valid(), "claim symbol is not valid");
    check(claimToken.quantity.amount > 0, "claim amount must be positive");
    check(duration >= 0 && duration < MAX_UINT32_T, "duration must be positive");

    // Insert
    _programs.emplace(get_self(), [&](auto& p) {
      p.index = _programs.available_primary_key();
      p.creator = creator;
      p.savedToken = extended_asset(0, claimToken.get_extended_symbol());
      p.claimToken = claimToken;
      p.duration = duration;
    });
  }

  void atom::update (
    const uint64_t& programId,
    const extended_asset& claimToken,
    const uint32_t& duration
  ) {
    // Find program
    auto programsIt = _programs.find(programId);
    check(programsIt != _programs.end(), "program does not exist");

    // Authenticate
    require_auth(programsIt->creator);

    // Params validation
    check(claimToken.quantity.is_valid(), "claim symbol is not valid");
    check(claimToken.quantity.amount > 0, "claim amount must be positive");
    check(claimToken.get_extended_symbol() == programsIt->claimToken.get_extended_symbol(), "claim token must be same as saved one");
    check(duration >= 0 && duration < MAX_UINT32_T, "duration must be positive");

    // Update
    _programs.modify(programsIt, get_self(), [&](auto& p) {
      p.claimToken = claimToken;
      p.duration = duration;
    });
  }

  void atom::del (
    const uint64_t& programId,
    const name& account
  ) {
    // Find program
    auto programsIt = _programs.find(programId);
    check(programsIt != _programs.end(), "program does not exist");
    check(programsIt->index != _programs.available_primary_key() - 1, "cannot delete latest program");
    
    // Authentication
    check(has_auth(get_self()) || has_auth(programsIt->creator), "invalid auth");

    // Refund
    if (programsIt->savedToken.quantity.amount > 0) {
      atom::transfer_action transfer_act(programsIt->savedToken.contract, {get_self(), name("active")});
      if (account != get_self()) {
        transfer_act.send(get_self(), account, programsIt->savedToken.quantity, "claim");
      }
    }

    // Delete
    _programs.erase(programsIt);
  }

  void atom::donate (
    const uint64_t& programId,
    const name& account,
    const extended_asset& token
  ) {
    // Find program
    auto programsIt = _programs.find(programId);
    check(programsIt != _programs.end(), "program does not exist");

    // Validate stake amount
    check(token.quantity.is_valid(), "quantity must be valid");
    check(token.quantity.amount > 0, "must stake positive amount");
    check(token.get_extended_symbol() == programsIt->savedToken.get_extended_symbol(), "extended symbol mismatch");

    // Add amount
    _programs.modify(programsIt, eosio::same_payer, [&](auto& p) {
      p.savedToken.quantity.amount += token.quantity.amount;
    });
  }

  void atom::claim (
    const uint64_t& programId,
    const name& account
  ) {
    // Authenticate
    require_auth(account);

    // Find program
    auto programsIt = _programs.require_find(programId, "program does not exist");

    // Calculate expiry
    uint32_t now = current_time_point().sec_since_epoch();
    uint32_t newExpiry = now + programsIt->duration;
    
    // Find account
    auto accountsIt = _accounts.find(account.value);

    if (accountsIt == _accounts.end()) {
      _accounts.emplace(get_self(), [&](auto& a) {
        a.account = account;
        a.nextExpiry = newExpiry;
        a.expiryByProgram = {{programsIt->index, newExpiry}};
      });
    } else {
      _accounts.modify(accountsIt, get_self(), [&](auto& a) {
        // Check if claimed before, and if so, current time past old expiry
        auto oldExpiry = a.expiryByProgram.find(programsIt->index);
        if (oldExpiry != a.expiryByProgram.end()) {
          check(oldExpiry->second < now, "claimed again too quickly");
        }

        // Save next expiry if closer
        if (newExpiry < a.nextExpiry) {
          a.nextExpiry = newExpiry;
        }

        // Save expiry
        a.expiryByProgram[programsIt->index] = newExpiry;
      });
    }

    // Quantity to claim (max savedToken)
    auto quantity = programsIt->claimToken.quantity;
    if (quantity > programsIt->savedToken.quantity) {
      quantity = programsIt->savedToken.quantity;
    }
    check(quantity.amount > 0, "claim quantity must be positive");

    // Reduce amount
    _programs.modify(programsIt, eosio::same_payer, [&](auto& p) {
      p.savedToken.quantity.amount -= quantity.amount;
    });

    // Send to claimer
    atom::transfer_action transfer_act(programsIt->claimToken.contract, {get_self(), name("active")});
    transfer_act.send(get_self(), account, quantity, "claim");

    // Retrieve ram
    process(10);
  }

  void atom::process (
    const uint8_t& maxProcess
  ) {
    uint32_t now = current_time_point().sec_since_epoch();
    auto idx = _accounts.get_index<"bynextexpiry"_n>();
    
    // iterate through descending expiries
    auto account = idx.lower_bound(now);
    for (auto i = 0; account != idx.end() && i < maxProcess; i++) {
      // If past expiry
      if (account->nextExpiry < now) {
        idx.modify(account, eosio::same_payer, [&](auto& a) {
          // Reset next expiry
          a.nextExpiry = MAX_UINT32_T;

          // Loop over all expiries for user
          std::map<uint64_t, uint32_t>::iterator it2 = a.expiryByProgram.begin();
          while (it2 != a.expiryByProgram.end()) {
            if (it2->second < now) {
              it2 = a.expiryByProgram.erase(it2);
            } else {
              if (it2->second < a.nextExpiry) {
                a.nextExpiry = it2->second;
              }

              ++it2;
            }
          }
        });

        // Delete if no next expiry
        if (account->nextExpiry == MAX_UINT32_T) {
          account = idx.erase(account);
        } else {
          account++;
        }
      }
    }
  }
} // namepsace contract
