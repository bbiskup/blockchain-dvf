#include "vendor/crow_all.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "blockchain.h"

namespace {
const boost::uuids::uuid nodeIdentifier{boost::uuids::random_generator()()};
}

int main() {
  bc::BlockChain blockChain{};

  crow::SimpleApp app;

  CROW_ROUTE(app, "/mine")([]() { return "test"; });

  app.port(5000).multithreaded().run();

  return 0;
}
