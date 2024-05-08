# POSE Bootcamp Code Quality Workshop: Address Book Example

This is a small example application for which we want to write unit tests using the [Catch2](https://github.com/catchorg/Catch2) unit testing framework.

## Building and Running

To build and run the application and tests, the following are required:
- CMake
- A modern C++ compiler (a few C++20 features are used, GCC 11.4 is sufficient)
- Python 3 (to run the HTTP synchronization server)

Configure the build using `cmake -S . -B build`, then use `cmake --build build` to compile.
This produces two executables, `address_book` and `address_book_tests` inside the `build` directory.
Both executables can be called without any arguments; the tests support various options for how to run them, run `address_book_tests --help` for more information.

To enable the synchronization feature, start the `simple_sync_server.py`.

## Specification

The `address_book` class defined in `src/address_book.hpp|.cpp` adheres to the following specification:

- Entries within the address book are identified based on their name.
- The address book provides functions for adding and removing entries based on their name, as well as a function to check whether an entry exists (`add_entry`, `remove_entry` and `has_entry`, respectively).
  - Names are case-insensitive and must be between 1 and 100 characters long.
  - Attempting to add an entry with a name violating these restrictions throws an exception.
- `address_book::get_entries` returns a list of all entries, sorted alphabetically.
- Names returned by the address book have the first letter of each word capitalized ("jane m doe" becomes "Jane M Doe").
- A phone number and birthday can be set for each entry.
  - Attempting to set a phone number or birthday on a non-existent entry throws an exception.
  - Attempting to set an invalid date as birthday throws an exception.
- The address book can be synchronized with a remote location using a *synchronization provider*.
  - The synchronization provider offers a single function `synchronize`, that is given a list of all local entries, serialized as `name,number,MM/DD`, for example `jane m doe,123456789,11/30`.
    It then returns a list in the same format after synchronization.
