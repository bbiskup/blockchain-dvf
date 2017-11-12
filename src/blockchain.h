#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <boost/optional.hpp>
#include <chrono>
#include <string>
#include <unordered_set>
#include <vector>

#include "vendor/json.hpp"
namespace bc {
using NodeAddr = std::string;
using Hash = std::string;
using TimeStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

struct Transaction {
  Transaction(std::string sender, std::string recipient, double amount);

  std::string sender;
  std::string recipient;
  double amount;
};

std::ostream& operator<<(std::ostream& strm, const Transaction& transaction);

struct Block {
  Block(size_t index, TimeStamp timeStamp,
        std::vector<Transaction> transactions, int proof, Hash previousHash);
  Block(Hash previousHash, int proof);
  size_t index;
  TimeStamp timeStamp;
  std::vector<Transaction> transactions;
  Hash previousHash;
  int proof;
};

void to_json(nlohmann::json& j, const bc::Block& block);
std::ostream& operator<<(std::ostream& strm, const Block& block);
using Chain = std::vector<Block>;

Hash hash(const Block& block);
Hash hash(const std::string& s);
bool validProof(int lastProof, int proof);

class BlockChain {
public:
  BlockChain();
  void registerNode(const NodeAddr address);
  bool validChain(const Chain& chain) const;
  bool resolveConflict();
  Block newBlock(int proof, const boost::optional<Hash>& previousHash);
  int newTransaction(const std::string& sender, const std::string& recipient,
                     double amount);
  const Block& lastBlock() const;
  int proofOfWork(int lastProof) const;

private:
  std::vector<Transaction> currentTransactions_;
  std::unordered_set<NodeAddr> nodes_;
  Chain chain_;
};
}; // namespace bc

#endif /* BLOCKCHAIN_H */
