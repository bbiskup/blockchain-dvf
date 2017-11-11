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

void bc::BlockChain::registerNode(const NodeAddr address) {
    nodes_.emplace(urlParse(address));
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
