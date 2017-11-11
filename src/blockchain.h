#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>

namespace bc{

    class Transaction{
        public:
            Transaction(std::string sender, std::string recipient, double amount);
        private:
            std::string sender_;
            std::string recipient_;
            double amount_;
    };

    class BlockChain{
        public:
        private:
    }
};

#endif  /* BLOCKCHAIN_H */
