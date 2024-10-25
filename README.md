# Object-Oriented-Design-Heuristics

Examples from the book "Object-Oriented Design Heuristics" by Arthur J. Riel, ISBN 978-0201633856

## Selected C++ Example # 21

**Overview Notes.** The following C++ example is an implementation of the ATM/Bank distributed process discussed in Chapter 11. It is a simulation in that there is no physical network between the two sides of the application. Each side was tested independently by simulating the network action through strings with the appropriate fields. The reader may replace four message sends in the `Network` class with their favorite byte-transfer mechanism system calls and have a fully distributed system through remote proxies. The actual mechaniusm used is irrelevant, as this code will demonstrate. Several points of interest in this code in which the reader may be particularly interested are itemized below. This code should not be examined until the reader has become familiarized with the design discussion of Chapter 11.

* This code is not particularly concerned with bells and whistles, although many are included. Undoubtedly, mroe could have been added. Items such as checking for corrupted input data files are fairly minimal. Any bells and whisltes that wre part of the function specification have been included, however. Physical network timeouts were not addressed in the code. They would normally be implemented via alarm signals on each of the processes. The Cancel key left the code in a cleaner state and therefore made it easier to understand. Their implementation is not conceptually difficult given this code, buit it is a bit tedious.
* The reader should be sure to examine the relationship between the `CardReader` and `PhysicalCardReader` classes in `atm.hpp` and `atm.cpp`. This is a good example of a wrapper class for software reuse. The comments surrounding these two classes discuss this reuse at length.
* The reader should also be sure to examine the role of the `SuperKeypad` class in allowing the containing `ATM` class to distribute the intelligence without adding uses relationships between its pieces. This type of abstraction is extremely useful to object-oriented designers and is discussed fully in the text portion of Chapter 11. It also is an example of a flaw in the data-driven methodologies. These methodologies can not detect these classes early in analysis and design for purposes of controlling the complexity of the design.
* The reader should carefully examine the role that the `BankProxy` (`atm.hpp`/`atm.cpp`), the `ATMProxy` (`bank.hpp`/`bank.cpp`), and the `Network` (`network.hpp`/`network.cpp`) classes play in hiding the details of distributed programming during design time. The user of remote proxies can clearly encapsulate the complex interprocess communication issues within distributed applications. This subject is discussed in Chapters 9 and 11.
* Lastly, efficiency issues were not a high priority. For those readl-time programmers who wish to question why I sened a four-byte ASCII field to report a Good/Bad status over the network instead of a single-byte (or bit), keep in mind this code is meant to teach design. Besides, four bytes make for a more extensible system!
* 
