#include "apiserver.h"
#include "vendor/catch.hpp"
#include "vendor/json.hpp"

TEST_CASE("Chain") {
  bc::Server server{5000};
  SECTION("fullChain") {
    auto j = nlohmann::json::parse(server.fullChain());
    nlohmann::json chainJson{j["chain"]};
    REQUIRE(chainJson.size() == 1);
    REQUIRE(chainJson[0]["index"] == 1);
    REQUIRE(chainJson[0]["previous_hash"] == "1");
    REQUIRE(chainJson[0].find("timestamp") != chainJson[0].end());
    REQUIRE(chainJson[0]["proof"] == 100);
    REQUIRE(chainJson[0]["transactions"].size() == 0);
  }
}
