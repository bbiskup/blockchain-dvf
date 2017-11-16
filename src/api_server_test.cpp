#include "apiserver.h"
#include "vendor/catch.hpp"
#include "vendor/crow_all.h"
#include "vendor/json.hpp"

TEST_CASE("Chain") {
  bc::Server server{5000};
    auto j = nlohmann::json::parse(server.fullChain());
    nlohmann::json chainJson{j["chain"]};
    REQUIRE(chainJson.size() == 1);
    REQUIRE(chainJson[0]["index"] == 1);
    REQUIRE(chainJson[0]["previous_hash"] == "1");
    REQUIRE(chainJson[0].find("timestamp") != chainJson[0].end());
    REQUIRE(chainJson[0]["proof"] == 100);
    REQUIRE(chainJson[0]["transactions"].size() == 0);
}

TEST_CASE("Transactions") {
  bc::Server server{5000};
    crow::request request{};
    request.body =
        R"({"sender": "test_sender", "recipient": "test_recipient", "amount": 10})";
    crow::response response = server.newTransaction(request);
    auto j = nlohmann::json::parse(response.body);
    REQUIRE(j["message"] == "Transaction will be added to Block 2");
}

TEST_CASE("Consensus") {
  bc::Server server{5000};
    auto j = nlohmann::json::parse(server.consensus());
    REQUIRE(j["message"] == "Our chain is authoritative");

    nlohmann::json chainJson{j["chain"]};
    REQUIRE(chainJson.size() == 1);
    REQUIRE(chainJson[0]["index"] == 1);
    REQUIRE(chainJson[0]["previous_hash"] == "1");
    REQUIRE(chainJson[0].find("timestamp") != chainJson[0].end());
    REQUIRE(chainJson[0]["proof"] == 100);
    REQUIRE(chainJson[0]["transactions"].size() == 0);
}

TEST_CASE("Mining") {
  bc::Server server{5000};
    auto j = nlohmann::json::parse(server.mine());
    REQUIRE(j["message"] == "New Block Forged");
    REQUIRE(j.find("previous_hash") !=j.end());

    nlohmann::json transJson{j["transactions"]};
    REQUIRE(transJson.size() == 1);
    REQUIRE(transJson[0]["amount"] == 1.0);
    REQUIRE(transJson[0]["sender"] == "0");
    REQUIRE(transJson[0].find("recipient") != transJson[0].end());
}

TEST_CASE("Node Registration") {
  bc::Server server{5000};
    crow::request request{};
    request.body =
        R"({"nodes": ["http://localhost:5001"]})";
    crow::response response = server.registerNodes(request);
    auto j = nlohmann::json::parse(response.body);
    REQUIRE(j["message"] == "New nodes have been added");
    REQUIRE(j["total_nodes"] == std::vector<std::string>{"localhost:5001"});
}
