# IP Address Management System

## Overview

This project implements an IP Address Management System using a Binary Search Tree (BST) structure. It stores and manages a list of IP addresses, each associated with an alias, and supports operations such as adding, updating, deleting, searching, and displaying information about IP addresses.

The system also validates IP addresses and aliases, logs errors, and allows the user to interact with it through a menu-driven interface.

## Features

1. **Add Address**: Add a new IP address and its alias to the system.
2. **Look Up Address**: Look up an IP address using its alias.
3. **Update Address**: Update the alias of an existing IP address.
4. **Delete Address**: Delete an address (and its alias) from the system.
5. **Display List**: Display all addresses stored in the system in an in-order traversal.
6. **Display Aliases for Location**: Display aliases for a given part of the IP address.
7. **Display Error Log**: Display the contents of the error log, which includes invalid inputs.
8. **Error Handling**: Invalid inputs such as incorrect IP formats, invalid aliases, and duplicate addresses are logged in an error log file.

## Getting Started

### Prerequisites

Ensure you have a C compiler (e.g., GCC) installed on your system. You will also need a text file (e.g., `CS531_Inet.txt`) containing IP addresses and aliases in the following format:

### Compilation

To compile the program, run the following command:
gcc -o ip_address_manager ip_address_manager.c

###  Running the Program
After compilation, you can run the program with the following command:
./ip_address_manager
