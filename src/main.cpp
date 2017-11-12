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
} // namespace

namespace bpo = boost::program_options;

// Endpoint handers
std::string mine();

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

  app.port(serverPort).multithreaded().run();

  return 0;
}
