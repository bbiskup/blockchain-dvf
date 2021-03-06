#include <cassert>
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

void bc::to_json(nlohmann::json& j, const bc::Transaction& t) {
  j = nlohmann::json{
      {"sender", t.sender}, {"recipient", t.recipient}, {"amount", t.amount}};
}

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
  auto epoch = block.timeStamp.time_since_epoch();
  auto secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
  j = nlohmann::json{{"index", block.index},
                     {"timestamp", secondsSinceEpoch},
                     {"transactions", block.transactions},
                     {"previous_hash", block.previousHash},
                     {"proof", block.proof}};
}

/// Creates a SHA-256 hash of a Block
/// \param block Block
bc::Hash bc::hash(const Block& block) {
  nlohmann::json blockJson = block;
  return hash(blockJson.dump());
}

bc::Hash bc::hash(const std::string& s) {
  char buf[SHA256_DIGEST_STRING_LENGTH];
  char* sTmp = const_cast<char*>(s.c_str());
  SHA256_Data(reinterpret_cast<unsigned char*>(sTmp), s.size(), buf);
  return buf;
}

/// Validates the Proof
/// \param lastProof Previous Proof
/// \param proof Current Proof
/// \return: true if correct, false if not.
bool bc::validProof(int lastProof, int proof) {
  std::string guess{std::to_string(lastProof) + std::to_string(proof)};
  std::string guessHash{hash(guess)};
  return guessHash.substr(0, 4) == "0000";
}

bc::Transaction::Transaction(std::string sender_, std::string recipient_,
                             double amount_)
    : sender{std::move(sender_)}, recipient{std::move(recipient_)},
      amount{amount_} {}

bc::Block::Block(size_t index_, TimeStamp timeStamp_,
                 std::vector<Transaction> transactions_, int proof_,
                 Hash previousHash_)
    : index{index_}, timeStamp{timeStamp_}, transactions{transactions_},
      previousHash{previousHash_}, proof{proof_} {}

bc::Block::Block(Hash previousHash_, int proof_)
    : previousHash{previousHash_}, proof{proof_} {}

bc::BlockChain::BlockChain() {
  // Create the genesis block
  newBlock(100, boost::optional<Hash>{"1"});
}

/// Add a new node to the list of nodes
/// \param address Address of node. Eg. "http://192.168.0.5:5000"
void bc::BlockChain::registerNode(const NodeAddr address) {
  nodes_.emplace(urlParse(address));
}

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
      std::cout << "Block hash incorrect" << std::endl;
      return false;
    }

    // Check that the Proof of Work is correct
    if (!validProof(lastBlock.proof, block.proof)) {
      std::cout << "Invalid proof" << std::endl;
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
bool bc::BlockChain::resolveConflicts() {
  const auto& neighbours = nodes_;
  boost::optional<Chain> newChain;

  // We're only looking for chains longer than ours
  size_t maxLength{chain_.size()};

  // Grab and verify the chains from all the nodes in our network
  for (const NodeAddr& node : neighbours) {
    nlohmann::json json{
        nlohmann::json::parse(getHTTP("http://" + node + "/chain"))};
    size_t length{json["length"]};
    Chain chain{chainFromJSON(json["chain"])};

    // Check if the length is longer and the chain is valid
    if (length > maxLength && validChain(chain)) {
      maxLength = length;
      std::cout << "#### new chain" << std::endl;
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

/// Create a new Block in the Blockchain
/// \param proof The proof given by the Proof of Work algorithm
/// \param previousHash Hash of previous Block
/// \return New Block
const bc::Block&
bc::BlockChain::newBlock(int proof, const boost::optional<Hash>& previousHash) {

  assert(!(!previousHash && chain_.empty()));
  std::string previousHashValue{previousHash ? *previousHash
                                             : hash(chain_.back())};

  Block block{chain_.size() + 1, std::chrono::high_resolution_clock::now(),
              currentTransactions_, proof, previousHashValue};

  // Reset the current list of transactions
  currentTransactions_.clear();

  chain_.emplace_back(block);
  return chain_.back();
}

// Creates a new transaction to go into the next mined Block

// \param sender Address of the Sender
// \param recipient Address of the Recipient
// \param amount Amount
// \return The index of the Block that will hold this transaction
size_t bc::BlockChain::newTransaction(const std::string& sender,
                                      const std::string& recipient,
                                      double amount) {
  currentTransactions_.emplace_back(sender, recipient, amount);
  return lastBlock().index + 1;
}

const bc::Block& bc::BlockChain::lastBlock() const { return chain_.back(); }

/// Simple proof of work algorithm:
/// - find a number p' such that hash(pp') contains leading 4 zeroes, where p
/// is the previous p'
/// - p is the previous proof, and p' is the new proof
int bc::BlockChain::proofOfWork(int lastProof) const {
  int proof{};
  while (!validProof(lastProof, proof)) {
    ++proof;
  }

  return proof;
}

const bc::Chain& bc::BlockChain::chain() const { return chain_; }
const std::unordered_set<bc::NodeAddr>& bc::BlockChain::nodes() const {
  return nodes_;
}

namespace {
std::string urlParse(const std::string& address) {
  std::match_results<std::string::const_iterator> m;
  if (regex_search(address, m, urlRegex)) {
    return std::string{m[2].first, m[2].second}; // netloc
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
