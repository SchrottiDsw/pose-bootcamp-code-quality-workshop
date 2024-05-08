#pragma once

#include "synchronization.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

constexpr size_t max_name_length = 100;

/// Returns the current date (month/day) as a `std::chrono::month_day` object.
std::chrono::month_day current_date();

class address_book {
  public:
	/// Adds an entry to the address book. Has no effect if the entry already exists.
	/// A name may not be empty or exceed `max_name_length` characters.
	void add_entry(std::string name);

	/// Removes an entry from the address book, if it exists.
	void remove_entry(std::string name);

	/// Returns whether an entry with the given name exists in the address book.
	bool has_entry(std::string name) const;

	/// Returns an alphabetically sorted list of all entries in the address book.
	std::vector<std::string> get_entries() const;

	/// Sets the phone number for an entry in the address book.
	/// Throws if the entry does not exist.
	void set_phone_number(std::string name, std::uint64_t number);

	/// Returns the phone number for an entry in the address book.
	/// Throws if the entry does not exist.
	std::uint64_t get_phone_number(std::string name) const;

	/// Sets the birthday for an entry in the address book.
	/// Throws if the entry does not exist or the birthday is invalid.
	void set_birthday(std::string name, std::chrono::month_day birthday);

	/// Returns the birthday for an entry in the address book.
	/// Throws if the entry does not exist.
	std::chrono::month_day get_birthday(std::string name) const;

	/// Returns the entry with the next birthday, relative to the current date.
	/// Throws if the address book is empty.
	std::string get_next_birthday() const;

	/// Synchronizes the address book with a remote provider.
	void synchronize(synchronization_provider& provider);

  private:
	struct entry {
		std::uint64_t phone_number;
		std::chrono::month_day birthday;
	};

	std::unordered_map<std::string, entry> m_entries;

	entry& get_entry(std::string name);

	const entry& get_entry(std::string name) const;
};