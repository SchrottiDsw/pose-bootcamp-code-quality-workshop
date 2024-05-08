#include <cctype>
#include <chrono>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "address_book.hpp"

TEST_CASE("entries can be added and removed") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("Jane Doe"));
	ab.remove_entry("Jane Doe");
	CHECK_FALSE(ab.has_entry("Jane Doe"));
}

TEST_CASE("names are case insensitive") {
	address_book ab;
	CHECK_FALSE(ab.has_entry("Jane Doe"));
	ab.add_entry("Jane Doe");
	CHECK(ab.has_entry("jane doe"));
	CHECK(ab.has_entry("JANE DOE"));
}

TEST_CASE("names must be between 1 and 100 chars") {
	address_book ab;
	CHECK_THROWS_WITH(ab.add_entry(""), "Name may not be empty");
	
	std::string name(101,'a');
	CHECK_THROWS_WITH(ab.add_entry(name), "Name too long");

	name = std::string(100,'a');
	ab.add_entry(name);
	CHECK(ab.has_entry(name));

	ab.add_entry("a");
	CHECK(ab.has_entry("a"));
}


TEST_CASE("getentries is alphabetically sorted") {
	address_book ab;
	
	ab.add_entry("c");
	ab.add_entry("a");
	ab.add_entry("b");
	auto entries = ab.get_entries();
	REQUIRE(entries.size()==3);

	for ( auto& entry : entries)
		std::transform(entry.begin(), entry.end(), entry.begin(),[](unsigned char c){ return std::tolower(c); });

	CHECK(entries[0]=="a");
	CHECK(entries[1]=="b");
	CHECK(entries[2]=="c");
}

TEST_CASE("first letters are capitalized") {
	address_book ab;
	
	ab.add_entry("jane m do");
	ab.add_entry("JANE M DOE");
	
	auto entries = ab.get_entries();
	REQUIRE(entries.size()==2);

	CHECK(entries[0]=="Jane M Do");
	CHECK(entries[1]=="Jane M Doe");
}

TEST_CASE("set phone number on entry") {
	address_book ab;

	ab.add_entry("Test");
	ab.set_phone_number( "Test", 123456789);

	CHECK(ab.get_phone_number("Test") == 123456789 );
}

TEST_CASE("set phone number on non existent entry") {
	address_book ab;
	
	CHECK_THROWS_WITH(ab.set_phone_number( "Test", 123456789), "Entry not found");
}

TEST_CASE("set birthday on entry") {
	address_book ab;

	ab.add_entry("Test");

	auto birthday = std::chrono::month_day{std::chrono::month{5}, std::chrono::day{5}};
	ab.set_birthday( "Test", birthday);

	CHECK(ab.get_birthday("Test") == birthday );
}

TEST_CASE("set birthday on non existent entry") {
	address_book ab;
	
	auto birthday = std::chrono::month_day{std::chrono::month{5}, std::chrono::day{5}};
	CHECK_THROWS_WITH(ab.set_birthday( "Test", birthday), "Entry not found");
}

TEST_CASE("set birthday validation") {
	address_book ab;

	ab.add_entry("Test");

	auto birthday = std::chrono::month_day{std::chrono::month{24}, std::chrono::day{100}};
	CHECK_THROWS_WITH(ab.set_birthday( "Test", birthday), "Invalid birthday");
}


