// Trans.hpp: (This file wasn't in the book)

#include "Trans.hpp"

void Transaction::update(const std::string &) const
{
}

bool Transaction::preprocess(const ATM &) const
{
    return true;
}

bool Transaction::postprocess(const ATM &) const
{
    return true;
}

std::string Transaction::packetize() const
{
    // TODO: Complete
    return std::string{};
}

Withdraw::Withdraw(const std::string &, const std::string &, double)
{
}

Deposit::Deposit(const std::string &, const std::string &, double)
{
}

Balance::Balance(const std::string &, const std::string &)
{
}

Transfer::Transfer(const std::string &, const std::string &, const std::string &, const std::string &)
{
}

TransactionList::TransactionList(unsigned int)
{
}

void TransactionList::print(const std::streambuf *) const
{
}

void TransactionList::addTransaction(const Transaction &)
{
}

void TransactionList::cleanup()
{
}
