#ifndef TRANS_HPP
#define TRANS_HPP

// Trans.hpp: This header file contains the definition of the
// classes that make up the Transaction portion of this
// application. The Transaction classes are interesting in that
// they span both sides of the application, i.e., they are used by
// both Bank classes and ATM classes. In fact, the transactions
// used by the Bank classes do not need ot look anything like the
// transactions used by the ATM classes. In this implemnentation, I
// have chosen to make them one and the same. The different public
// interfaces are supported by #ifdef's using ATM_SIDE and
// BANK_SIDE macros to differentiate them.

class ATM;

#include <ctime>
#include <ostream>
#include <string>
#include <vector>

// A TimeStamp object encapsulates the date and time of a
// transaction. The current implementation is that of a string
// produced via the time and ctime library routines. The reader
// should feel to come up with something more elaborate.

class TimeStamp
{
    std::string dateTime;

public:
    TimeStamp();

    static std::ostream &operator<<(std::ostream &, const TimeStamp &);
};

// All transactions have a TimeStamp, one account name, its PIN
// and an amount. (Note: While Balnace transactions do not
// need an amount, they use it to carry back the balance value.)
// Recall that protected accessor methods are perfectly
// appropriate. The packetize method is used to turn the object
// into a string suitable for shipping across the network. It
// requires every transaction (derived classes included) to know
// the format oif this string, but the alternative is to have the
// Transaction class grant friendship to the network or at least
// provide a colleciton of accessor methods. The latter tends to be
// problematic in that all classes gain access ot the abstract
// implemnentation of the Transaction class. This format knowledge
// does not imply that a network needs to ship the format as-is. It
// implies only that they must use it to produce the final packets
// they will ship.

class Transaction
{
    TimeStamp timeStamp;
    std::string sourceAccount;
    std::string pin;
    double amount;

protected:
    Transaction(const std::string &, const std::string &, double);

    std::string getSourceAccount() const;
    void setAmount(double);

public:
    virtual void print();
    virtual std::ostream &operator<<(std::ostream &, const Transaction &);

    // Only ATM classes use the proprocess, postprocess, and update
    // methods. The Transaction class for the Bank side of the
    // application never compiles them into the object code.

#ifdef ATM_SIDE
    virtual bool preprocess(const ATM &) const;
    virtual bool postprocess(const ATM &) const;
    virtual void update(const std::string &) const;
    virtual std::string packetize() const;
#endif

    // The process and verify accoutn methods are used only by the
    // classes on the Bank's side of the application. The Transaction
    // class for the ATM side of the application never compiles them
    // into the object code.

#ifdef BANK_SIDE
    virtual bool process(const AccountList &) = 0;
    bool verifyAccount(Iconst Account &);
    virtual std::string packetize() const;
#endif
};

// The following four classes are the derived classes of the
// abstract Transaction class: Deposit, Withdraw, Balance, and
// Transfer.

class Deposit : public Transaction
{
public:
    Deposit(const std::string &, const std::string &, double);
    void print() override;
    std::string type() override;

#ifdef ATM_SIDE
    bool preprocess(const ATM &) const override;
#endif

#ifdef BANK_SIDE
    bool process(const AccountList &) override;
#endif
};

class Withdraw : public Transaction
{
public:
    Withdraw(const std::string &, const std::string &, double);
    std::string type() override;

#ifdef ATM_SIDE
    bool preprocess(const ATM &) const override;
    bool postprocess(const ATM &) const override;
#endif

#ifdef BANK_SIDE
    void print() override;
    ;
#endif
};

class Balance : public Transaction
{
    double balance;

public:
    Balance(const std::string &, const std::string &);
    void print() override;
    std::string type() override;

#ifdef ATM_SIDE
    void update(const std::string &) const override;
#endif

#ifdef BANK_SIDE
    bool process(const AccountList &) override;
#endif
};

// The Transfer class adds the additional data member of
// targetAccount since the money needs to be transferrred
// someplace.

class Transfer : public Transaction
{
    std::string targetAccount;

public:
    Transfer(const std::string &, const std::string &, const std::string &, const std::string &);

    void print() override;
    std::string type() override;

#ifdef ATM_SIDE
    std::string packetize() const override;
#endif

#ifdef BANK_SIDE
    bool process(const AccountList &) override;
#endif
};

class TransactionList
{
    std::vector<Transaction> transList;

public:
    TransactionList(unsigned int);

    void addTransaction(const Transaction &);
    void print(const std::streambuf *) const;
    void cleanup();
};

#endif
