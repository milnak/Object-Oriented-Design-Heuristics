#ifndef TRANS_HPP
#define TRANS_HPP

// Trans.hpp: (This file wasn't in the book)

class ATM;

#include <string>

class Transaction
{
public:
    Transaction(char, const std::string &);

    void update(const std::string &) const;
    bool preprocess(const ATM &) const;
    bool postprocess(const ATM &) const;
    std::string packetize() const;
};

class Withdraw : public Transaction
{
public:
    Withdraw(const std::string&, const std::string &, double);
};

class Deposit : public Transaction
{
public:
    Deposit(const std::string&, const std::string &, double);
};

class Balance : public Transaction
{
public:
    Balance(const std::string&, const std::string &);
};

class Transfer : public Transaction
{
public:
    Transfer(const std::string&, const std::string &, const std::string &, const std::string &);
};

class TransactionList
{
public:
    TransactionList(unsigned int);

    void print(const std::streambuf *) const;
    void addTransaction(const Transaction &);
    void cleanup();
};

#endif
