// Trans.cpp: The source code for the Transaction classes. The
// Transaction classes are interesting in that appear on  both
// sides of the application. The Transaction objects on each side
// of the application could be completely different definitions.
// For convenience, they were made the same in this example. The
// parts of the public interface unique to one side of thje
// application are restricted through the use of the BANK_SIDE or
// ATM_SIDE macros.

#ifdef ATM_SIDE
#include "atm.hpp"
#endif

#ifdef BANK_SIDE
#include "bank.hpp"
#endif

#include "trans.hpp"

#include <chrono>

// The TimeStamp constructor uses the time and ctime standard C
// library functions to create a simple string capturing date and
// time.

TimeStamp::TimeStamp()
{
    std::time_t result = std::time(nullptr);
    dateTime = std::ctime(&result);
}

std::ostream &TimeStamp::operator<<(std::ostream &stream, const TimeStamp &timeStamp)
{
    stream << timeStamp.dateTime;
    return stream;
}

Transaction::Transaction(const std::string &account, const std::string &p, double a) : sourceAccount(account),
                                                                                       pin(p),
                                                                                       amount(a)
{
}

std::string Transaction::getSourceAccount() const
{
    return sourceAccount;
}

void setAmount(double newAmount)
{
    amount = newAmount;
}

std::ostream & Transaction::operator<<(std::ostream &stream, const Transaction &timeStamp)
{
    // TODO: %4.2lf
    stream << timeStamp << "\tAccount: " << sourceAccount << "\tAmount: " << amount << std::endl;
}

#ifdef ATM_SIDE

bool Transaction::preprocess(const ATM& atm) const
{
    return true;
}

void Transaction::postprocess(const ATM&) const
{
}

void Transaction::update(const std::string&, int ) const
{
}

void Transaction::packetize() const
{
    string buf;
    buf += type();
    buf += sourceAccount;
    buf += " ";
    buf += pin;
    buf += amount; // TODO: %4.2lf
}
#endif

// If this is the bank side of the application, include a
// verifyAccount method, which checks if an account's name and
// PIN match that of the Transaction.

#ifdef BANK_SIDE

#endif
