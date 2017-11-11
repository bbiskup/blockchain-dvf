#include "blockchain.h"

bc::Transaction::Transaction(std::string sender_, std::string recipient_, double amount_)
    : sender{std::move(sender_)}, recipient{std::move(recipient_)}, amount{amount_}{}
