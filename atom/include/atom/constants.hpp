#pragma once

// Token constants
#define TOKEN_SYMBOL_CODE_RAW "XPR"
#define TOKEN_CONTRACT_RAW "eosio.token"
#define TOKEN_PRECISION 4

#define TOKEN_SYMBOL_CODE eosio::symbol_code(TOKEN_SYMBOL_CODE_RAW)
#define TOKEN_CONTRACT eosio::name(TOKEN_CONTRACT_RAW)
#define TOKEN_SYMBOL eosio::symbol(TOKEN_SYMBOL_CODE, TOKEN_PRECISION)

namespace proton
{
  static constexpr auto WORD_SIZE = 32u;
} // namespace proton