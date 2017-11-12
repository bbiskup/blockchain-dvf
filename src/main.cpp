#include "vendor/crow_all.h"
#include "vendor/json.hpp"
#include <boost/program_options.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <string>

#include "blockchain.h"

namespace {
const std::string nodeIdentifier{
    boost::uuids::to_string(boost::uuids::random_generator()())};
const unsigned short defaultServerPort{5000};
bc::BlockChain blockChain{};
const short jsonIndent{4};

const int badRequest{400};
} // namespace

namespace bpo = boost::program_options;

// Endpoint handers
std::string mine();
crow::response newTransaction(const crow::request request);

std::string mine() {
  // We run the proof of work algorithm to get the next proof...
  const bc::Block& lastBlock = blockChain.lastBlock();
  int lastProof = lastBlock.proof;
  int proof = blockChain.proofOfWork(lastProof);

  // We must receive a reward for finding the proof.
  // The sender is "0" to signify that this node has mined a new coin.
  const bc::Block& block = blockChain.newBlock(proof, lastBlock.previousHash);

  nlohmann::json response{{"message", "New Block Forged"},
                          {"index", block.index},
                          {"transactions", block.transactions},
                          {"proof", block.proof},
                          {"previous_hash", block.previousHash}};
  return response.dump(jsonIndent);
}

/// Create a new Transaction
crow::response newTransaction(const crow::request request) {
  nlohmann::json values = nlohmann::json::parse(request.body);

  try {
    size_t index{blockChain.newTransaction(
        values["sender"], values["recipient"], values["amount"])};

    nlohmann::json response{{"message", "Transaction will be added to Block " +
                                            std::to_string(index)}};
    return response.dump();
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return crow::response{badRequest};
  }
}

int main(int argc, char** argv) {
  bpo::options_description desc{"Allowed options:"};
  bool is_help{false};
  unsigned short serverPort;
  desc.add_options()("help,help",
                     bpo::bool_switch(&is_help)->default_value(false),
                     "Display help message")(
      "port,p",
      bpo::value<unsigned short>(&serverPort)->default_value(defaultServerPort),
      "port to listen on");
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
  bpo::notify(vm); // may throw

  if (is_help) {
    std::cerr << desc << std::endl;
    return 0;
  }

  crow::SimpleApp app;

  CROW_ROUTE(app, "/mine")(mine);
  CROW_ROUTE(app, "/transactions/new").methods("POST"_method)(newTransaction);

  app.port(serverPort).multithreaded().run();

  return 0;
}
