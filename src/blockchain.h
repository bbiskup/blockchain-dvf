#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>

namespace bc{

    class Transaction{
        public:
            Transaction(std::string sender, std::string recipient, double amount);
        private:
            const std::string sender;
            const std::string recipient;
            const double amount;
    };

    class BlockChain{
        public:
        private:
    };
};

#endif  /* BLOCKCHAIN_H */
