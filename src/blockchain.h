#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <boost/optional.hpp>
#include <string>
#include <unordered_set>
#include <vector>


#include "vendor/json.hpp"
namespace bc {
using NodeAddr = std::string;
using Hash = std::string;

struct Block {
   Hash previousHash; 
   int proof;
};

void to_json(nlohmann::json& j, const bc::Block& block);

std::ostream& operator<<(std::ostream& strm, const Block& block);

using Chain = std::vector<Block>;

struct Transaction {
  Transaction(std::string sender, std::string recipient, double amount);

  const std::string sender;
  const std::string recipient;
  const double amount;
};

std::ostream& operator<<(std::ostream& strm, const Transaction& transaction);

Hash hash(const Block& block);
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
  std::vector<Transaction> transactions_;
  std::unordered_set<NodeAddr> nodes_;
  Chain chain_;
};
}; // namespace bc

#endif /* BLOCKCHAIN_H */
