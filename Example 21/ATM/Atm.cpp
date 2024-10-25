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
    new_p.append(name + "." + std::to_string(count))}

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
    catch (const std::ifstream::failure& e)
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

    const string account{ buf.substr(0, 7) };

    if (badAccount(account))
    {
        physicalCardReader.ejectCard();
        return false;
    }

    // If the PIN is bad, return 1 and eject.

    const string pin{ buf.substr(8, 4) };

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

bool CardReader::getAccount(string& acc) const
{
    if (validCard)
    {
        acc = account;
    }

    return validCard;
}

bool CardReader::getPin(string& p) const
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

void DisplayScreen::displayMsg(const string& msg)
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

void SuperKeypad::displayMsg(const string & msg)
{
    displayScreen->displayMsg(msg)

}

// The verify_pin method enables the keypad, prompts the user
// for a PIN, and checks it against the user-supplied
// PIN. The method returns zero on success, nonzero
// on failure.

int SuperKeypad::verifyPin(const string & pinToVerify)
{
    keypad->enable();
    DisplayScreen->displayMsg("Enter Pin Number: ")
    string pin;
    // TODO: better way of doing this? algorithm?
    while (true) {
        c = keypad->getkey();
        if (c == '\n')
        {
            break;
        }

        pin += c;
    }
    std::getline(std::cin, pin);
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

Transaction *SuperKeypad::getTransaction(const string &, const string &);
{
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
        // TODO: while (keypad->getKey() != EnterKey);
    } while (transType != 'W' && transType != 'D' &&
        transType != 'B' && transType != 'T' && transType != 'Q');

    if transType == 'Q')
    {
        return NULL;
    }
}
