#include <chrono>
#include <iostream>
#include <limits>
#include <string>

#include "address_book.hpp"

void clear_line() { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

std::string get_name() {
	printf("Enter name: ");
	std::string name;
	std::getline(std::cin, name);
	return name;
}

void add_entry(address_book& ab) {
	std::string name = get_name();
	ab.add_entry(name);

	uint64_t number;
	std::cout << "Enter phone number: ";
	scanf("%lu", &number);
	clear_line();
	ab.set_phone_number(name, number);

	std::cout << "Enter birthday (MM/DD): ";
	unsigned int month = 0;
	unsigned int day = 0;
	scanf("%u/%u", &month, &day);
	clear_line();
	ab.set_birthday(name, std::chrono::month_day{std::chrono::month{month}, std::chrono::day{day}});
}

void remove_entry(address_book& ab) {
	std::string name = get_name();
	ab.remove_entry(name);
}

void list_entries(address_book& ab) {
	auto entries = ab.get_entries();
	for(std::string entry : entries) {
		std::chrono::month_day birthday = ab.get_birthday(entry);
		printf("%s %lu %u/%u\n", entry.c_str(), ab.get_phone_number(entry), (unsigned)birthday.month(),
		    (unsigned)birthday.day());
	}
}

void next_birthday(address_book& ab) {
	std::string name = ab.get_next_birthday();
	std::chrono::month_day birthday = ab.get_birthday(name);
	printf("%s's birthday is on %u/%u\n", name.c_str(), (unsigned)birthday.month(), (unsigned)birthday.day());
}

void synchronize(address_book& ab) {
	http_synchronization_provider provider{"http://localhost:3333"};
	ab.synchronize(provider);
	printf("Synchronized %zu entries\n", ab.get_entries().size());
}

int main() {
	address_book ab;

	printf("Address Book\n");
	while(true) {
		printf("\n(A)dd entry, (R)emove entry, (L)ist entries, (N)ext birthday, (S)ynchronize, (Q)uit\n");

		try {
			char choice;
			scanf("%c", &choice);
			clear_line();
			switch(choice) {
			case 'A':
			case 'a': add_entry(ab); break;

			case 'R':
			case 'r': remove_entry(ab); break;

			case 'L':
			case 'l': list_entries(ab); break;

			case 'N':
			case 'n': next_birthday(ab); break;

			case 'S':
			case 's': synchronize(ab); break;

			case 'Q':
			case 'q': return 0;

			default: printf("Invalid choice\n"); break;
			}
		} catch(const std::exception& e) { printf("Error: %s\n", e.what()); }
	}

	return 0;
}