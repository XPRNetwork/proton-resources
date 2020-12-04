#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "eosio.system_tester.hpp"
#include <eosio/chain/exceptions.hpp>

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>
#include <iostream>
#include <filesystem>
#include <iostream>
#include <array>
#include <vector>

namespace fs = std::filesystem;

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

class proton_tester : public tester {
public:
   proton_tester() {
      // Create accounts
      produce_blocks( 2 );
      create_accounts({
         N(eosio.token),
         N(proton),
         N(atom)
      });
      produce_blocks( 2 );

      // Deploy token
      set_code( N(eosio.token), contracts::token_wasm() );
      set_abi( N(eosio.token), contracts::token_abi().data() );
      create_currency( N(eosio.token), N(proton), asset(100000000000000, symbol{CORE_SYM}) );
      issue( asset(10000000000000, symbol{CORE_SYM}), N(proton) );
      produce_blocks();

      // Add token ABI to ABI serializer
      const auto& token_acct = control->db().get<account_object, by_name>( N(eosio.token) );
      abi_def token_abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(token_acct.abi, token_abi), true);
      abi_ser.set_abi(token_abi, abi_serializer_max_time);
      produce_blocks();

      // Deploy user contract (atom)
      set_code( N(atom), contracts::atom_wasm() );
      set_abi( N(atom), contracts::atom_abi().data() );
      produce_blocks();

      // Add atom ABI to ABI serializer
      const auto& wasm_accnt = control->db().get<account_object, by_name>( N(atom) );
      abi_def wasm_abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(wasm_accnt.abi, wasm_abi), true);
      abi_ser.set_abi(wasm_abi, abi_serializer_max_time);
      produce_blocks();
   }

   // Create token
   void create_currency( name contract, name manager, asset maxsupply ) {
      auto act =  mutable_variant_object()
         ("issuer",       manager )
         ("maximum_supply", maxsupply );

      base_tester::push_action(contract, N(create), contract, act );
   }

   // Issue token
   void issue( const asset& amount, const name& manager = config::system_account_name ) {
      base_tester::push_action( N(eosio.token), N(issue), manager, mutable_variant_object()
                                ("to",       manager )
                                ("quantity", amount )
                                ("memo",     "")
                                );
   }

   // Transfer token
   void transfer( const name& from, const name& to, const asset& amount, const std::string& memo = "" ) {
      base_tester::push_action(
         N(eosio.token),
         N(transfer),
         from,
         mutable_variant_object()
            ("from",    from)
            ("to",      to )
            ("quantity", amount)
            ("memo", memo)
      );
   }

   abi_serializer abi_ser;
};


BOOST_AUTO_TEST_SUITE(proton_test)
   BOOST_FIXTURE_TEST_CASE( test_case1, proton_tester ) try {
      // Transfer token to contract to deposit
      transfer(N(proton), N(atom), asset::from_string("200.0000 XPR"), "Hello");

      // Check condition
      BOOST_REQUIRE_EQUAL( "ok", "ok" );

   } FC_LOG_AND_RETHROW()
BOOST_AUTO_TEST_SUITE_END()
