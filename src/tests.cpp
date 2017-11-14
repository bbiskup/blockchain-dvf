#define CATCH_CONFIG_MAIN
#include "blockchain.h"
#include "vendor/catch.hpp"
#include "vendor/json.hpp"

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
    REQUIRE(block.transactions.empty());
  }
};

TEST_CASE("Block") {
  bc::Block block{"test_hash", 99};
  block.index = 10;
  block.transactions = {};

  nlohmann::json j;
  bc::to_json(j, block);
  REQUIRE(j.dump() == R"({"index":10,"previous_hash":"test_hash","proof":99,"timestamp":0,"transactions":[]})");
}
