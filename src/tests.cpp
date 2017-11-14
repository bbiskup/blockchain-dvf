#define CATCH_CONFIG_MAIN
#include "blockchain.h"
#include "vendor/catch.hpp"

TEST_CASE("BlockChain") {
  bc::BlockChain bc;

  SECTION("Chain") {
    const bc::Chain& chain{bc.chain()};
    REQUIRE(chain.size() == 1);
    REQUIRE(chain.at(0).index == 1);
  }

  SECTION("Genesis block") {
    const bc::Block& block{bc.lastBlock()};
    REQUIRE(block.proof == 100);
    REQUIRE(block.previousHash == "1");
    REQUIRE(block.index == 1);
  }
};
