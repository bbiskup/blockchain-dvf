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
  REQUIRE(
      j.dump() ==
      R"({"index":10,"previous_hash":"test_hash","proof":99,"timestamp":0,"transactions":[]})");
}

TEST_CASE("hash") {
  REQUIRE(bc::hash("") ==
          "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  REQUIRE(bc::hash("x") ==
          "2d711642b726b04401627ca9fbac32f5c8530fb1903cc4db02258717921a4881");
  REQUIRE(bc::hash("Satoshi_Nakamoto") ==
          "175027194c0b650bf9053eabdd1a1bcb02ac41d0c4800ca39a8ab1501a84dcec");
}
TEST_CASE("validProof") { REQUIRE(bc::validProof(100, 35293)); }

TEST_CASE("Transaction") {
  bc::Transaction transaction{"test_sender", "test_recipient", 100};

  nlohmann::json j;
  bc::to_json(j, transaction);
  REQUIRE(
      j.dump() ==
      R"({"amount":100.0,"recipient":"test_recipient","sender":"test_sender"})");
}
