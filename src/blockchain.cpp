#include <curl_easy.h>
#include <curl_exception.h>
#include <curl_ios.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

#include "blockchain.h"
#include "vendor/crow_all.h"

#include "vendor/sha2-1.0.1/sha2.h"

namespace {
std::string urlParse(const std::string& address);
std::string getHTTP(const std::string& url);

// Courtesy https://stackoverflow.com/a/3624766
const std::regex urlRegex{"^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$"};
bc::Chain chainFromJSON(const nlohmann::json& json);
} // namespace

std::ostream& bc::operator<<(std::ostream& strm, const Block& block) {
  return strm << "Block prev:" << block.previousHash
              << ", proof:" << block.proof;
}

std::ostream& bc::operator<<(std::ostream& strm, const bc::Transaction& t) {
  return strm << "sender:" << t.sender << ", recipient:" << t.recipient
              << ", amount" << t.amount;
}

/// JSON conversion functions
void bc::to_json(nlohmann::json& j, const bc::Block& block) {
  j = nlohmann::json{{"previous_hash", block.previousHash},
                     {"proof", block.proof}};
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

/// Creates a SHA-256 hash of a Block
/// \param block Block
bc::Hash bc::hash(const Block& block) {
  char buf[SHA256_DIGEST_LENGTH];
  // We must make sure that the Dictionary is Ordered,
  // or we'll have inconsistent hashes
  // json.dumps(block, sort_keys=True).encode();
  nlohmann::json blockJson = block;
  std::string blockJsonStr{blockJson.dump()};
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  // TODO return hashlib.sha256(blockString).hexdigest();
  char* blockJsonStrC = const_cast<char*>(blockJsonStr.c_str());
  SHA256_Update(&ctx, reinterpret_cast<unsigned char*>(blockJsonStrC),
                blockJsonStr.size());
  SHA256_End(&ctx, buf);
  return buf;
}
bool bc::validProof(int lastProof, int proof) {
  throw std::runtime_error{"not implemented: validProof"};
}
#pragma clang diagnostic pop

bc::Transaction::Transaction(std::string sender_, std::string recipient_,
                             double amount_)
    : sender{std::move(sender_)}, recipient{std::move(recipient_)},
      amount{amount_} {}

/// Add a new node to the list of nodes
/// \param address Address of node. Eg. "http://192.168.0.5:5000"
void bc::BlockChain::registerNode(const NodeAddr address) {
  nodes_.emplace(urlParse(address));
}

bc::BlockChain::BlockChain() {}

/// Determine if a given blockchain is valid
/// \param chain A blockchain
/// \return true if valid, false if not
bool bc::BlockChain::validChain(const Chain& chain) const {
  size_t chainLen{chain.size()};
  if (chainLen == 0) {
    return false;
  }
  Block lastBlock{chain[0]};

  for (size_t currentIndex{1}; currentIndex < chainLen; ++currentIndex) {
    const Block& block{chain[currentIndex]};
    std::cout << "Last block: " << nlohmann::json{lastBlock} << "\n";
    std::cout << "Block: " << nlohmann::json{block} << "\n";
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
  std::cout << std::flush;
  return true;
}

/// This is our consensus algorithm, it resolves conflicts
/// by replacing our chain with the longest one in the network.
/// \return true if our chain was replaced, false if not
bool bc::BlockChain::resolveConflict() {
  const auto& neighbours = nodes_;
  boost::optional<Chain> newChain;

  // We're only looking for chains longer than ours
  size_t maxLength{chain_.size()};

  // Grab and verify the chains from all the nodes in our network
  for (const NodeAddr& node : neighbours) {
    nlohmann::json json{getHTTP("http://" + node + "/chain")};

    size_t length{json["length"]};
    Chain chain{chainFromJSON(json["chain"])};

    // Check if the length is longer and the chain is valid
    if (length > maxLength && validChain(chain)) {
      maxLength = length;
      newChain = chain;
    }
  }

  // Replace our chain if we discovered a new, valid chain longer than ours
  if (newChain) {
    chain_ = *newChain;
    return true;
  }

  return false;
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

std::string getHTTP(const std::string& url) {
  std::ostringstream response;
  curl::curl_ios<std::ostringstream> writer{response};
  curl::curl_easy easy{writer};
  easy.add<CURLOPT_URL>(url.c_str());
  easy.add<CURLOPT_FOLLOWLOCATION>(1L);
  easy.perform();
  return response.str();
}

bc::Chain chainFromJSON(const nlohmann::json& json) {
  bc::Chain result;
  std::transform(
      json.begin(), json.end(), std::back_inserter(result),
      [](const nlohmann::json& itemJson) {
        return bc::Block{itemJson["previous_hash"], itemJson["proof"]};
      });
  return result;
}
} // namespace
