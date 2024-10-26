// Atm.cpp: The source file of the main classes composing the ATM
// side of the application. It consists of all methods and global
// data definitions required by these classses.

#include "network.hpp"
#include "atm.hpp"
#include "trans.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>

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

bool badAccount(const std::string &account)
{
    return std::all_of(account.begin(), account.end(), ::isdigit);
}

// For now PIN and account nubmers use the same algorithm.
// They may drift in the future.

bool isBadPin(const std::string &pin)
{
    return badAccount(pin);
}

// Each PhysicalCardReader has a name, which is usees as the name of
// the BankCard file when it is inserted into CardSlot directory.
// This naming would not be necessary in a real system, since the
// hardware would take card of this naming problem. It appears in
// this application onlyh for simplifying the simulation.

PhysicalCardReader::PhysicalCardReader(const std::string &n) : name(n)
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

std::string PhysicalCardReader::readinfo() const
{
    std::filesystem::path file{cardSlots.append(name)};

    // TODO: catch (const std::system_error & e) and check e.code()
    std::ifstream ifs(file);
    ifs.exceptions(std::ios::failbit | std::ifstream::badbit);

    std::string info;
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

    std::filesystem::remove(file);
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
    new_p.append(name + "." + std::to_string(count));
    std::filesystem::rename(old_p, new_p);
}

// The constructor for CardReader calls the constructor of its
// PhysicalCardReader.

CardReader::CardReader(const std::string &name) : physicalCardReader(name)
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

bool CardReader::readCard()
{
    std::string buf;

    try
    {
        buf = physicalCardReader.readinfo();
    }
    catch (const std::ifstream::failure &)
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

    if (badAccount(buf.substr(0, 7)))
    {
        physicalCardReader.ejectCard();
        return false;
    }

    // If the PIN is bad, return 1 and eject.

    if (isBadPin(buf.substr(8, 4)))
    {
        physicalCardReader.ejectCard();
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

std::string CardReader::getAccount() const
{
    if (validCard)
    {
        return account;
    }

    // TODO: Throw InvalidCard exception?
    return std::string{};
}

std::string CardReader::getPin() const
{
    if (validCard)
    {
        return pin;
    }

    // TODO: Throw InvalidCard exception?
    return std::string{};
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

char Keypad::getKey() const
{
    // TODO: getchar()
    return enabled ? static_cast<char>(std::getchar()) : '\0';
}

void DisplayScreen::displayMsg(const std::string &msg)
{
    std::cout << "@ATM Display@ " << msg << std::endl;
}

SuperKeypad::SuperKeypad()
{
    keypad = std::make_unique<Keypad>();
    displayScreen = std::make_unique<DisplayScreen>();
}

// This method delegates to contained display screen. Such
// noncommunicating behavior is an argument for splitting the
// SuperKeypad class. However, the verify_pin() and
// get_transaction() methods provide more than enough cohesion
// of data to justify the SuperKeypad's existence.

void SuperKeypad::displayMsg(const std::string &msg)
{
    displayScreen->displayMsg(msg);
}

// The verify_pin method enables the keypad, prompts the user
// for a PIN, and checks it against the user-supplied
// PIN. The method returns zero on success, nonzero
// on failure.

bool SuperKeypad::verifyPin(const std::string &pinToVerify)
{
    keypad->enable();
    displayScreen->displayMsg("Enter Pin Number: ");
    std::string pin;
    char c;
    while ((c = keypad->getKey()) != EnterKey)
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

std::unique_ptr<Transaction> SuperKeypad::getTransaction(const std::string &account, const std::string &pin)
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

    displayScreen->displayMsg("Enter Account Type (S/C): ");
    transactionAccount += keypad->getKey();
    while (keypad->getKey() != EnterKey) /*nothing*/
        ;

    if (transType != 'B')
    {
        displayScreen->displayMsg("Enter Amount: ");
        std::string amount_str;
        char c;
        while ((c = keypad->getKey()) != EnterKey)
        {
            amount_str += c;
        }
        amount = std::atof(amount_str.c_str());
    }

    std::string targetAccount;

    if (transType == 'T')
    {
        displayScreen->displayMsg("Enter Target Account Number: ");
        char c;
        while ((c = keypad->getKey()) != EnterKey)
        {
            targetAccount += c;
        }

        displayScreen->displayMsg("Enter Target Account Type (S/C): ");
        targetAccount += keypad->getKey();
    }

    switch (transType)
    {
    case 'W':
        return std::make_unique<Withdraw>(transactionAccount, pin, amount);
    case 'D':
        return std::make_unique<Deposit>(transactionAccount, pin, amount);
    case 'B':
        return std::make_unique<Balance>(transactionAccount, pin);
    case 'T':
        return std::make_unique<Transfer>(transactionAccount, pin, targetAccount, account);
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

void ReceiptPrinter::print(const TransactionList &transactionList)
{
    std::cout << "@@ReceiptPrinter@ Your receipt is as follows:" << std::endl;

    std::streambuf *buf;
    std::ofstream ofs;

    // Attempt to write to "receipt". If that fails, write to stdout.
    try
    {
        ofs.exceptions(std::ios::failbit | std::ifstream::badbit);
        ofs.open("receipt");
        buf = ofs.rdbuf();
    }
    catch (const std::ifstream::failure &)
    {
        buf = std::cout.rdbuf();
    }

    transactionList.print(buf);
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

BankProxy::BankProxy(std::unique_ptr<Network> &n)
{
    network = std::move(n);
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

bool BankProxy::process(const Transaction &t)
{
    if (!network->send(t))
    {
        return false;
    }

    int status;

    const std::string other_info{network->receive(status)};
    if (!other_info.empty())
    {
        t.update(other_info);
    }

    return status;
}

// A new ATM object is given its Bank Proxy, a name to be handed down
// to its PhysicalCardReader (only needed for a simulation), and its
// initial cash.

ATM::ATM(std::unique_ptr<BankProxy> &b, const std::string &name, unsigned int cash)
{
    bankProxy = std::move(b);
    cardReader = std::make_unique<CardReader>(name);
    superKeypad = std::make_unique<SuperKeypad>();
    cashDispenser = std::make_unique<CashDispenser>(cash);
    depositSlot = std::make_unique<DepositSlot>();
    receiptPrinter = std::make_unique<ReceiptPrinter>();
    transactionList = std::make_unique<TransactionList>(MaxTransactionAtm);
}

// The activate method for the ATM class is the main driver for the
// ATM objects. This method puts up the welcome message and waits
// for a card to become available (in simulation, a card becomes
// available when a user copies a file with the PhysicalCardReader's
// name into the CardSlots directory). When a card is
// available, the ATM retrieves the account and PIN from the
// user and ensuring that it equals the one from the card. The
// actual check will be done by the Bank, which ensures that the PIN
// is equal to the one stored in the Account object.
// Once the PIN is verified, this method asks the SuperKeypad to
// collect and build a transaction. It preprocesses the
// transaction (handling things like getting envelopes for
// deposits, checking the cash dispenser to ensure enough cash is
// available for a withdrawal, etc.). If preprocessing was
// successful, it then asks its BankProxy to process the
// transaction. This method packages up the transaction, ships it
// over the network to the Bank's application, and collects a
// response from the Bank's application via the network. Notice
// the transparent nature of the interprocess communication. At
// design time we were able to completely ignore this distributed
// processing. Assuming the processing went well, we then execute
// a postprocess, which performs tasks like giving the user his or
// her money, etc. This method repeats the processing of the
// transaction until the user selects Quit, which requires the
// SuperKeypad::get_transaction method to return NULL. At this
// time the receipt printer generates a receipt and ejects the
// card.

void ATM::activate()
{
    while (true)
    {
        superKeypad->displayMsg("Welcome to the Bank of Heuristics!");
        superKeypad->displayMsg("Please Insert Your Card In the Card Reader");

        // Get a card.
        while (cardReader->readCard())
        {
            const std::string account{cardReader->getAccount()};
            const std::string pin{cardReader->getPin()};

            // Try three times to verify the PIN.
            unsigned int count = 0;
            bool verified;
            do
            {
                verified = superKeypad->verifyPin(pin);

            } while (!verified && count++ < 3);

            // If it couldn't be verified,then eat the card.
            if (!verified)
            {
                superKeypad->displayMsg("Sorry, three strikes and you're out!");
                cardReader->eatCard();
            }
            else
            {
                // Otherwise, keep getting Transactions until the user asks to
                // quit.

                std::unique_ptr<Transaction> transaction;
                while ((transaction = superKeypad->getTransaction(account, pin)) != NULL)
                {
                    // Preprocess the transaction, if necessary. The default is to do
                    // nothing.
                    if (transaction->preprocess(*this))
                    {
                        // If preprocessing was successful, then process the Transaction.
                        // If the Bank says the Transaction is valid, then add it to the
                        // current list (for the receipt) and carry out any postprocessing.

                        if (bankProxy->process(*transaction))
                        {
                            transactionList->addTransaction(*transaction);
                            transaction->postprocess(*this);
                        }
                    }
                    else
                    {
                        // If problems occur, display an appropriate message and continue.
                        superKeypad->displayMsg("The Bank Refuses Your Transaction");
                        superKeypad->displayMsg("Contact your Bank Representative.");
                    }
                }
            }

            // When we're done, print the receipt, clean up the Transaction
            // list, and eject the card. We're now ready to loop for another user.
            receiptPrinter->print(*transactionList);
            transactionList->cleanup();
            cardReader->ejectCard();
        }
    }
}

// These are methods used by derived types of Transaction,
// specifically, in their pre-/post-process methods.

bool ATM::retrieveEnvelope()
{
    return depositSlot->retrieveEnvelope();
}

bool ATM::enoughCash(double amount)
{
    return cashDispenser->enoughCash(static_cast<unsigned int>(amount));
}

bool ATM::dispenseCash(double amount)
{
    return cashDispenser->dispense(static_cast<unsigned int>(amount));
}
