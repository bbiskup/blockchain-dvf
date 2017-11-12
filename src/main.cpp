#include "vendor/crow_all.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/program_options.hpp>
#include <iostream>

#include "blockchain.h"

namespace {
const boost::uuids::uuid nodeIdentifier{boost::uuids::random_generator()()};
const unsigned short defaultServerPort{5000};
} // namespace


namespace bpo = boost::program_options;

int main(int argc, char** argv) {
  bpo::options_description desc{"Allowed options:"};
  bool is_help{false};
  unsigned short serverPort;
  desc.add_options()("help,help",
                     bpo::bool_switch(&is_help)->default_value(false),
                     "Display help message")(
      "port,p", bpo::value<unsigned short>(&serverPort)->default_value(defaultServerPort),
      "port to listen on");
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
  bpo::notify(vm); // may throw

  if (is_help) {
    std::cerr << desc << std::endl;
    return 0;
  }
  bc::BlockChain blockChain{};

  crow::SimpleApp app;

  CROW_ROUTE(app, "/mine")([]() { return "test"; });

  std::cout << "Starting server on port " << serverPort << std::endl;
  app.port(serverPort).multithreaded().run();

  return 0;
}
