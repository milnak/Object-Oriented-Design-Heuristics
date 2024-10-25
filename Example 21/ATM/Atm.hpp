// Atm.hpp: This is the header file for the main classes, which
// reside solely on the ATM side of the application.

#ifndef ATM_HPP
#define ATM_HPP

// Files that deal with transactions will often define an ATM_SIDE
// or BANK_SIDE macro to describe which application space they are
// in. The transaction code gets compiled into each application.
// but some methods are only appropriate for one address space or
// the other.

#include <filesystem>
#include <string>

#include "consts.hpp"

// These two pointers provide the path for the Card
// Reader's directory, which simulates where a card is
// inserted, and the ATM's directory, where eaten cards
// are placed. Normally these would be hardware addresses,
// but for our simulation they are going to be directories
// in a file system. A person copies a BankCard file
// into the CardSlots directory to simulate inserting
// a card into the ATM and the ATM removes the file to its
// simulate ejecting the card. The ATM moves the filoe to its
// internal directory to simulate eating a card. The Bank
// Card file is a one-line ASCII file that consists of seven
// numeric characters representing the account number and
// a four-digit PIN number. The bank card itself is the
// same name as the physical card reader's "name" data member.
// These two directory strings are assigned int he main driver
// function to the first two command line arguments passed
// in.

extern std::filesystem::path cardSlots;
extern std::filesystem::path atmSlots;

// The relationship between the PhysicalCardReader
// and the CardReader is that the CardReader is a
// wrapper class for the PhysicalCardReader. The
// latter is very reusable in that it reads only raw
// data off a card. The CardReader is specific to
// the domain of ATM and therefore is not reusable. It is
// responsible for distributing the system intelligence
// from the ATM to its pieces (in this case, the CardReader).

class PhysicalCardReader
{
    std::string name;

public:
    PhysicalCardReader(const string &);

    string readinfo() const;
    void ejectCard() const;
    void eatCard() const;
};

// The following clases model the physical pieces of the ATM
// class. They include CardReader, Display, Keypad, DepositSloit,
// CashDispenser, and ReceiptPrinter. Of particular interest is
// the SuperKeypad. This class exists solely due to an analysis of
// the behavior of the system. This analysis indicatedc much
// cohesion between the Keypad and DisplayScreen. Cohesion is one
// metric for determinign when two or more classes need
// encapsulation inside of a containing class. It is useful to note
// that a pure data driven analysis methodology would be unable to
// detect such classes since they are motivated by behavior and do
// not exist in the real-world model.

class CardReader
{
    PhysicalCardReader physicalCardReader;
    bool validCard{false};
    std::string account;
    std::string pin;

public:
    CardReader(const string &);

    bool readCard() const;
    bool getAccount(string &) const;
    bool getPin(string &) const;
    void ejectCard() const;
    void eatCard() const;
};

class Keypad
{
    bool enabled{false};

public:
    Keypad();

    void enable();
    void disable();
    char getKey() const;
};

class DisplayScreen
{
public:
    void displayMsg(const string &);
}

class SuperKeypad
{
    unique_ptr<Keypad> keypad;
    unique_ptr<DisplayScreen> displayScreen;

public:
    SuperKeypad();

    void displayMsg(const string &);
    int verifyPin(const string &);
    Transaction *getTransaction(const string &, const string &);
};

class CashDispenser
{
    unsigned int cashOnHand;

public:
    CashDispenser(unsigned int);
    bool enoughCash(unsigned int);
    bool dispense(unsigned int);
};

class DepositSlot
{
public:
    bool retrieveEnvelope();
};

class ReceiptPrinter
{
public:
    void print(const TransactionList &);
};

// The BankProxy class is the representative of the Bank class in
// the ATM's address space. It is a wrapper class for the Network,
// which is itself a wrapper for the exact byte-transfer mechanism
// (pipes, in this example).

class BankProxy
{
    // TODO: should this be a shared_pointer? What's ownership?
    Network &network;

public:
    BankProxy(Network &);
    bool process(const Transaction &);
};

class ATM
{
    BankProxy *bankProxy;
    CardReader *cardReader;
    SuperKeypad *superKeypad;
    CashDispenser *cashDispenser;
    DepositSlot *depositSlot;
    ReceiptPrinter *receiptPrinter;
    TransactionList *transactionList;

public:
    ATM(BankProxy *, const string &, int);
    ~ATM();

    void activate();
    int retrieveEnvelope();
    int enoughCash(double);
    int dispenseCash(double);
};

#endif
