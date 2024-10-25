// Atm.cpp: The source file of the main classes composing the ATM
// side of the application. It consists of all methods and global
// data definitions required by these classses.

#include "network.h"
#include "atm.hpp"
#include "trans.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

// Definition of the two card slots in the ATM system, the
// card reader's slot, where a user inserts his or her card,
// and the ATM's slot into which eaten cards are taken.
// These are simulated in this system by directories in the
// file system. The two paths names are given to the ATM application
// as its first two command-line parameters.

std::filesystem::path cardSlots;
std::filesystem::path atmSlots;

// The checking of an account name determines that iut consists of
// numeric digits. (Note: The actual account on the Bank side of the
// application sees seven digits plus a terminating S or C for
// savings and checking, respectively).

bool badAccount(const string &account)
{
    return std::all_of(account.begin(), account.end(), ::isdigit);
}

// For now PIN and account nubmers use the same algorithm.
// They may drift in the future.

bool isBadPin(const string &pin)
{
    return badAccount(pin);
}

// Each PhysicalCardReader has a name, which is usees as the name of
// the BankCard file when it is inserted into CardSlot directory.
// This naming would not be necessary in a real system, since the
// hardware would take card of this naming problem. It appears in
// this application onlyh for simplifying the simulation.

PhysicalCardReader::PhysicalCardReader(const string &n) : name(n)
{
}

// The readInfo method tries to open a file in the CardSlots
// directory with the name of the card reader. This name
// would not be needed in a real system. The encoded data
// would be read off the card reader's hardware. The method
// returns a one if the card cannot be read, zero if it
// was read successfully. The buf argument is fille in
// with the first line of the file on success. It is assumed
// to contain a seven-digit numeric string (account number)
// followed by a four-digit numering string (PIN).

string PhysicalCardReader::readinfo() const
{
    std::filesystem::path file{cardSlots.append(name)};

    std::ifstream ifs(file);
    ifs.exceptions(std::ios::failbit);

    string info;
    std::getline(ifs, info);

    return info;
}

// The simulation for eject cards is to remove the file from the card
// slot directory. In a real ATM system, this method would be a call
// to a hardware driver.

void PhysicalCardReader::ejectCard() const
{
    std::filesystem::path file{cardSlots};
    file.append(name);

    std::filesystem::remove(file)
}

// The simulation for eating cards is to move the BankCard file from
// the CardSlot directory to the ATM slot directory. In a real ATM
// system, this method would be a call to a hardware driver.

void PhysicalCardReader::eatCard() const
{
    static int count = 0;

    ++count;

    std::filesystem::path old_p{cardSlots};
    old_p.append(name);

    std::filesystem::path new_p{atmSlots};
    new_p.append(name + "." + std::to_string(count))
}

std::filesystem::rename(old_p, new_p);
}

// The constructor for CardReader calls the constructor of its
// PhysicalCardReader.

CardReader::CardReader(const string &name) : physicalCardReader(name)
{
}

// The read_card method checks to see if there is a card
// in the slot. If there isn't, it returns 1. If there is and
// it isn't readable, then the card is rejected and a 1 is
// returned. If the data on the card is readable, thent the account
// and PIN are read from the card (account is assumed to be
// a seven-character numeric string, the PIN a four-digit numeric
// string). If the data cannot be parsed (the card is not a valid
// bank card), then a 1 is returend and the card is ejected.

bool CardReader::readCard() const
{
    string buf;

    try
    {
        buf = physicalCardReader.readinfo();
    }
    catch (const std::ifstream::failure &e)
    {
        // TODO: How to differentiate file not found from short read?

        // If the card couldn't be read, then eject it.
        physicalCardReader.ejectCard();
        return false;

        // If there is no card, then report it to ATM.
        return false;
    }

    // We have the information, parse it.
    // If the account number is bad, return 1 and eject.

    const string account{buf.substr(0, 7)};

    if (badAccount(account))
    {
        physicalCardReader.ejectCard();
        return false;
    }

    // If the PIN is bad, return 1 and eject.

    const string pin{buf.substr(8, 4)};

    if (isBadPin(pin)
    {
        PhysicalCardReader.ejectCard();
        return false;
    }

    validCard = true;

    return true;
}

// The accessor methods are required for verifying a user-
// supplied PIN and building transactions. These are
// valid since there is a design situation facing policy between
// two separate key abstractions, i.e., the SuperKeypad and
// the CardReader. Both return 0 on success, 1 on failure.

bool CardReader::getAccount(string &acc) const
{
    if (validCard)
    {
        acc = account;
    }

    return validCard;
}

bool CardReader::getPin(string &p) const
{
    if (validCard)
    {
        p = pin;
    }

    return validCard;
}

// The following two methods simply delegate to their wrapped
// PhysicalCardReader class and execute its methods.

void CardReader::ejectCard() const
{
    physicalCardReader.ejectCard();
}

void CardReader::eatCard() const
{
    physicalCardReader.eatCard();
}

Keypad::Keypad()
{
}

void Keypad::enable()
{
    // TODO:  fflush(stdin);
    enabled = true;
}

void Keypad::disable()
{
    // TODO: fflush(stdin);
    enabled = false;
}

// The getkey method reads a single character from the Keypad
// (in the simulation, the hardware is assumed to be the standard
// input). We assume the newline character to be the Enter key,
// implying that all input has been received. The method returns
// the character read on success, NULL terminator if the keypad
// is not enabled.

char Keypad::getKey()
{
    // TODO: getchar()
    return enabled ? std::getchar() : '\0';
}

void DisplayScreen::displayMsg(const string &msg)
{
    std::cout << "@ATM Display@ " << msg << std::endl;
}

SuperKeypad::SuperKeypad()
{
    keypad = make_unique<Keypad>();
    displayScreen = make_unique<DisplayScreen>();
}

// This method delegates to contained display screen. Such
// noncommunicating behavior is an argument for splitting the
// SuperKeypad class. However, the verify_pin() and
// get_transaction() methods provide more than enough cohesion
// of data to justify the SuperKeypad's existence.

void SuperKeypad::displayMsg(const string &msg)
{
    displayScreen->displayMsg(msg)
}

// The verify_pin method enables the keypad, prompts the user
// for a PIN, and checks it against the user-supplied
// PIN. The method returns zero on success, nonzero
// on failure.

int SuperKeypad::verifyPin(const string &pinToVerify)
{
    keypad->enable();
    DisplayScreen->displayMsg("Enter Pin Number: ")
        string pin;
    while ((char c = keypad->getKey()) != EnterKey)
    {
        pin += c;
    }
    keypad->disable();
    return pin == pinToVerify;
}

// Note the case analysis on the type of transaction. This case
// analysis is necessary since our object-oriented design has
// bumped up against an action-oriented (text-menu driven) user
// interface as per our discussion in Chapter 9. At least this case
// analysis is restricted to one point in the design (one method)
// and hidden in the SuperKeypad class. Any classes higher in the
// system are oblivious to the case analysis.

Transaction *SuperKeypad::getTransaction(const string &account, const string &pin);
{
    std::string transactionAccount(account);
    char transType;
    double amount;

    keypad->enable();
    do
    {
        displayScreen->displayMsg("Select a Transaction");
        displayScreen->displayMsg("  W)ithdrawal");
        displayScreen->displayMsg("  D)eposit");
        displayScreen->displayMsg("  B)alance");
        displayScreen->displayMsg("  T)ransfer");
        displayScreen->displayMsg("  Q)uit");
        transType = keypad->getKey();
        while (keypad->getKey() != EnterKey) /*nothing*/
            ;
    } while (transType != 'W' && transType != 'D' &&
             transType != 'B' && transType != 'T' && transType != 'Q');

    if (transType == 'Q')
    {
        return NULL;
    }

    DisplayScreen->displayMsg("Enter Account Type (S/C): ");
    transactionAccount += keypad->getKey();
    while (keypad->getKey() != EnterKey) /*nothing*/
        ;

    if (transType != 'B')
    {
        DisplayScreen->displayMsg("Enter Amount: ");
        std::string amount_str;
        while ((char c = keypad->getKey()) != EnterKey)
        {
            amount_str += c;
        }
        amount = std::atof(amount_str);
    }

    if (transType == 'T')
    {
        display->displayMsg("Enter Target Account Number: ");
        std::string target_account;
        while ((char c = keypad->getKey()) != EnterKey)
        {
            target_account += c;
        }

        display->displayMsg("Enter Target Account Type (S/C): ");
        target_account += keypad->getKey();
    }
    switch (transType)
    {
    case 'W':
        return make_unique<Withdraw>(transactionAccount, pin, amount);
    case 'D':
        return make_unique<Deposit>(transactionAccount, pin, amount);
    case 'B':
        return make_unique<Balance>(transactionAccount, pin);
    case 'T':
        return make_unique<Transfer>(transactionAccount, pin, targetAccount, account);
    default:
        std::cerr << "Unknown type in get_transaction switch statement" << std::endl;
        return NULL;
    }
}

CashDispenser::CashDispenser(unsigned int initialCash) : cashOnHand{initialCash}
{
}

bool CashDispenser::enoughCash(unsigned int amount)
{
    return amount <= cashOnHand;
}

// We can give out only multiples of $10. The reader may want to
// elaborate on this class by giving it fixed numbers of $20 bills,
// $10s, $5's, etc. Some ATMs allow for the dispensing of stamps,
// theater tickets, etc., as well. Many warn the user that they are
// out of $10 bills and will dispense only multiples of $20. All of
// these items can be added to this class without impact on the rest
// of the system.

bool CashDispenser::dispense(unsigned int amount)
{
    amount -= amount % 10;
    if (enoughCash(amount))
    {
        std::cout << "@CashDispenser@ Giving the user " << amount << " cash" << std::endl;
        return true;
    }

    return false;
 }


bool DepositSlot::retrieveEnvelope()
{
    std::cout << "@DepositSlot@ Getting an envelope from the user" << std::endl;
    return true;
}

// The receipt printer simulates the printing of receipts by
// creationg a Receipts file in the current working directory.
// Again, the reader can elaborate on this class, adding a number
// of error checks, paper availability, etc. Like the cash
// dispenser, this is left as an exercise to th e reader since it
// adds no pedagogical benefit to this example.

void ReceiptPrinter::print(const TransactionList& translist)
{
    std::cout << "@@ReceiptPrinter@ Your receipt is as follows:" << std::endl;

    // TODO: Write to file "receipt", or if that fails, write to stdout
    translist.print(fd);
}

// The BankProxy is an extremely important class. It is the
// representatikve of the Bank class within the ATM application. It
// is merely a wrapper for the Network class, which is a wrapper
// itself for which tarnsport mechanism a distributed process is
// going to use for communication. In this example, I chose to
// simulate the network, but readers are free to sue any network
// or byte-transfer mechanism they wish. The application is
// completely independent of this mechanism. (Note: Changes in the
// byte-transfer mechanism affect only the Network class's
// implementation.)

BankProxy::BankProxy(Network& n)
{
    network = n;
}

// When a BankProxy needs to process a transaction, it asks its
// Network object to send it. Assuming the send works correctly,
// the method then asks the Network for a response, which takes the
// form of a status integer (0, 1, indicating success or failure on
// part of the real Bank class living in the bank's application
// space). If other Transaction application-specific data is
// required, then it is sent to the appropriate transaction's
// update message. This is to allow the Bank to update the state of
// a transaction in the ATM's application space from changes
// generated from the Bank's application space. Currently, only
// the Balance derived transaction users this method to update it's
// balance from the account in the Bank's application space.

bool BankProxy::process(const Transaction& t)
{
    if(!network.send(t))
    {
        return false;
    }

    int status;

    count = network.receive(status, other_info);
    if (count)
    {
        t.update(other_info, count);
    }

    return status;
}
