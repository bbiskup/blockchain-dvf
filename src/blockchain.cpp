#include <iostream>
#include <regex>
#include <stdexcept>

#include "blockchain.h"
#include "vendor/crow_all.h"

namespace {
std::string urlParse(const std::string& address);

// Courtesy https://stackoverflow.com/a/3624766
const std::regex urlRegex{"^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$"};
} // namespace

bc::Transaction::Transaction(std::string sender_, std::string recipient_,
                             double amount_)
    : sender{std::move(sender_)}, recipient{std::move(recipient_)},
      amount{amount_} {}

/// Add a new node to the list of nodes
/// \param address: Address of node. Eg. "http://192.168.0.5:5000"
void bc::BlockChain::registerNode(const NodeAddr address) {
  nodes_.emplace(urlParse(address));
}

/// Determine if a given blockchain is valid
/// \param chain: A blockchain
/// \return true if valid, false if not
bool bc::BlockChain::validChain(const Chain& chain) const {
  size_t chainLen{chain.size()};
  if (chainLen == 0) {
    return false;
  }
  Block lastBlock{chain[0]};

  for (size_t currentIndex{1}; currentIndex < chainLen; ++currentIndex) {
    // TODO print(f'{last_block}')
    // TODO print(f'{block}')
    std::cout << "\n-----------\n";

    // Check that the hash of the block is correct
    if (block.previousHash != hash(lastBlock)) {
      return false;
    }

    // Check that the Proof of Work is correct
    if (!validProof(lastBlock.proof, block.proof)) {
      return false;
    }
    lastBlock = block;
    currentIndex += 1;
  }
  return true;
}

/// This is our consensus algorithm, it resolves conflicts
/// by replacing our chain with the longest one in the network.
/// \return true if our chain was replaced, false if not
bool bc::BlockChain::resolveConflict(){
}

namespace {
std::string urlParse(const std::string& address) {
  std::match_results<std::string::const_iterator> m;
  if (regex_search(address, m, urlRegex)) {
    return std::string{m[1].first, m[1].second}; // netloc
  } else {
    throw std::runtime_error{"Unable to parse URL " + address};
  }
}
} // namespace
