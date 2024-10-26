// Network.hpp: The header file for the network implementation
// class. While not a first-class citizen in this design (i.e., a key
// abstraction), it is a very important implementation class. This
// class is a basic wrapper for the physical network, whatever it
// happens to be (a pipe, sockets, a LAN, a WAN, a telephone, two
// soup cans). The data of the Network class can be filled in with the
// reader's favorite mechanism. The methods of the class clearly
// state where a send buffer or receive buffer needs to be placed.
// This code has a simulation so that both sidse of the application
// can be tested.

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <memory>
#include <string>

class Transaction;

class Network
{
    // The user's favorite byte-transmission method goes here. See the
    // implementation of the four methods to determine where the send
    // and receive for this method need to go.

public:
#ifdef ATM_SIDE
    bool send(const Transaction &);
    std::string receive(int &);
#endif

#ifndef BANK_SIDE
    std::unique_ptr<Transaction> receive();
    void send(int, const Transaction &);
#endif
};

#endif
