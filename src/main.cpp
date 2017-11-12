#include "vendor/crow_all.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <iostream>

#include "blockchain.h"

namespace {
//const boost::uuids::uuid nodeIdentifier{boost::uuids::random_generator()()};
const int serverPort{5000};
} // namespace

int main() {
  bc::BlockChain blockChain{};

  crow::SimpleApp app;

  CROW_ROUTE(app, "/mine")([]() { return "test"; });

  std::cout << "Starting server on port " << serverPort << std::endl;
  app.port(serverPort).multithreaded().run();

  return 0;
}
