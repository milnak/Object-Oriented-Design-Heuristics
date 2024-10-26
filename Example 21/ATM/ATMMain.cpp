// ATMMain.cpp: The main driving routine for the ATM side of the
// application. This main function initializes the Card Slot
// directory (to simulate the card slot of the card reader), and ATM
// Slot directory (to simulate the ATM slot where the ATM puts the
// cards it eats). These are initialized from two command line
// arguments, which are passed in to the main program. It then
// builds a network object, which would require some parameter to
// initialize a particular byte-transmission mechanism in the
// real world. For this simulation, the user will type in strings to
// simulate information transmitted over a network of some kind.
// The main method then builds BankProxy around the network and
// uses it to create an ATM object. It then activates the ATM
// object, which sits in an infinite loop waiting for bank cards.

#include <iostream>

#include "network.hpp"
#include "atm.hpp"
#include "trans.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " CardSlots ATMSlots" << std::endl;
        return 1;
    }

    std::unique_ptr<Network> network{std::make_unique<Network>()};
    std::unique_ptr<BankProxy> myBank{std::make_unique<BankProxy>(network)};
    std::unique_ptr<ATM> atm{std::make_unique<ATM>(myBank, "ATM1", 8500.00)};

    cardSlots = argv[1];
    atmSlots = argv[2];

    atm->activate();

    return 0;
}
