#include "apiserver.h"
#include "vendor/json.hpp"
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

namespace {
const std::string nodeIdentifier{
    boost::uuids::to_string(boost::uuids::random_generator()())};
// avoid necessity to syncronize access to blockchain
constexpr unsigned short serverConcurrency{1};
constexpr short jsonIndent{4};

// HTTP response codes
namespace http {
const int badRequest{400};
const int created{201};
} // namespace http
} // namespace

bc::Server::Server(unsigned short serverPort) {
  // Lambda wrappers required as Crow does not support
  // pointers to member functions at this point.
  CROW_ROUTE(app_, "/mine")([this]() { return mine(); });
  CROW_ROUTE(app_, "/transactions/new")
      .methods("POST"_method)([this](const crow::request& request) {
        return newTransaction(request);
      });
  CROW_ROUTE(app_, "/chain")([this]() { return fullChain(); });
  CROW_ROUTE(app_, "/nodes/register")
      .methods("POST"_method)([this](const crow::request& request) {
        return registerNodes(request);
      });
  CROW_ROUTE(app_, "/nodes/resolve")([this]() { return consensus(); });
  app_.port(serverPort).concurrency(serverConcurrency);
}

void bc::Server::run() { app_.run(); }

std::string bc::Server::mine() {
  // We run the proof of work algorithm to get the next proof...
  const bc::Block& lastBlock = blockChain_.lastBlock();
  int lastProof = lastBlock.proof;
  int proof = blockChain_.proofOfWork(lastProof);

  // We must receive a reward for finding the proof.
  // The sender is "0" to signify that this node has mined a new coin.
  blockChain_.newTransaction("0", nodeIdentifier, 1);

  // Forge the new Block by adding it to the chain
  const bc::Block& block = blockChain_.newBlock(proof, boost::none);

  nlohmann::json response{{"message", "New Block Forged"},
                          {"index", block.index},
                          {"transactions", block.transactions},
                          {"proof", block.proof},
                          {"previous_hash", block.previousHash}};
  return response.dump(jsonIndent);
}

/// Create a new Transaction
crow::response bc::Server::newTransaction(const crow::request request) {
  nlohmann::json values = nlohmann::json::parse(request.body);

  try {
    size_t index{blockChain_.newTransaction(
        values["sender"], values["recipient"], values["amount"])};

    nlohmann::json response{{"message", "Transaction will be added to Block " +
                                            std::to_string(index)}};
    return response.dump(jsonIndent);
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return crow::response{http::badRequest};
  }
}

std::string bc::Server::fullChain() {
  nlohmann::json response{{"chain", blockChain_.chain()},
                          {"length", blockChain_.chain().size()}};
  return response.dump(jsonIndent);
}

std::string bc::Server::consensus() {
  try {
    bool replaced{blockChain_.resolveConflicts()};

    nlohmann::json response{{"message", replaced
                                            ? "Our chain was replaced"
                                            : "Our chain is authoritative"},
                            {"chain", blockChain_.chain()}};

    return response.dump(jsonIndent);
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    throw;
  }
}

crow::response bc::Server::registerNodes(const crow::request& request) {
  nlohmann::json values = nlohmann::json::parse(request.body);

  try {
    nlohmann::json& nodes = values["nodes"];
    if (nodes.empty()) {
      return {http::badRequest, "Error: Please supply a valid list of nodes"};
    }

    for (const auto& node : nodes) {
      blockChain_.registerNode(node);
    }

    std::vector<bc::NodeAddr> nodesVec{blockChain_.nodes().begin(),
                                       blockChain_.nodes().end()};
    std::sort(nodesVec.begin(), nodesVec.end());

    nlohmann::json response{
        {"message", "New nodes have been added"},
        {"total_nodes", nodesVec},
    };
    return {http::created, response.dump(jsonIndent)};
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return crow::response{http::badRequest};
  }
}
