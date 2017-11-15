#ifndef API_SERVER_H
#define API_SERVER_H

#include "blockchain.h"
#include "vendor/crow_all.h"
#include <string>

namespace bc {
class Server {
public:
  Server(unsigned short serverPort);
  std::string mine();
  crow::response newTransaction(const crow::request request);
  std::string fullChain();
  std::string consensus();
  crow::response registerNodes(const crow::request& request);

  void run();

private:
  crow::SimpleApp app_;
  bc::BlockChain blockChain_{};
};

}; // namespace bc

#endif /* API_SERVER_H */
