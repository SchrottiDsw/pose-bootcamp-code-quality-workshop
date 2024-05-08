#include "address_book.hpp"

#include <algorithm>
#include <ctime>
#include <stdexcept>

/// Normalizes a name by converting it to lowercase.
std::string normalize_name(std::string name) {
	std::string lowercase_name = name;
	std::transform(name.begin(), name.end(), lowercase_name.begin(), ::tolower);
	return lowercase_name;
}

/// Capitalizes the first letter of each word in a name.
std::string prettify_name(std::string name) {
	std::string pretty_name = name;
	for(std::size_t i = 0; i < pretty_name.size(); ++i) {
		if(i == 0 || pretty_name[i - 1] == ' ') { pretty_name[i] = std::toupper(pretty_name[i]); }
	}
	return pretty_name;
}

std::chrono::month_day current_date() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(now);
	// Half of C++20 chrono calendar is NYI, we have to fall back to C
	tm local_tm = *std::localtime(&tt);
	return std::chrono::month_day{std::chrono::month{static_cast<unsigned int>(local_tm.tm_mon + 1)},
	    std::chrono::day{static_cast<unsigned int>(local_tm.tm_mday)}};
}

void address_book::add_entry(std::string name) {
	if(name.empty()) { throw std::invalid_argument("Name may not be empty"); }
	if(name.size() > max_name_length) { throw std::invalid_argument("Name too long"); }
	if(name.find(',') != std::string::npos) { throw std::invalid_argument("Name may not contain ','"); }
	m_entries.try_emplace(normalize_name(name), entry{});
}

void address_book::remove_entry(std::string name) { m_entries.erase(normalize_name(name)); }

bool address_book::has_entry(std::string name) const { return m_entries.contains(normalize_name(name)); }

std::vector<std::string> address_book::get_entries() const {
	std::vector<std::string> entries;
	for(const auto& [name, _] : m_entries) {
		entries.push_back(prettify_name(name));
	}
	std::sort(entries.begin(), entries.end());
	return entries;
}

void address_book::set_phone_number(std::string name, std::uint64_t number) { get_entry(name).phone_number = number; }

std::uint64_t address_book::get_phone_number(std::string name) const { return get_entry(name).phone_number; }

// Use C++20 chrono calendar for validation (::ok() method). also use
void address_book::set_birthday(std::string name, std::chrono::month_day birthday) {
	if(!birthday.ok()) { throw std::invalid_argument("Invalid birthday"); }
	get_entry(name).birthday = birthday;
}

std::chrono::month_day address_book::get_birthday(std::string name) const { return get_entry(name).birthday; }

std::string address_book::get_next_birthday() const {
	if(m_entries.empty()) { throw std::invalid_argument("No birthdays set"); }

	std::string name;
	std::chrono::month_day today = current_date();

	std::chrono::months min_months{12};
	std::chrono::days min_days{31};

	for(const auto& [n, e] : m_entries) {
		std::chrono::months mdiff = e.birthday.month() - today.month();
		std::chrono::days ddiff = e.birthday.day() - today.day();
		if(mdiff.count() == 0 && e.birthday.day() < today.day()) { mdiff = std::chrono::months{12}; }
		if(mdiff < min_months || (mdiff == min_months && ddiff < min_days)) {
			name = n;
			min_months = mdiff;
			min_days = ddiff;
		}
	}
	return prettify_name(name);
}

void address_book::synchronize(synchronization_provider& provider) {
	std::vector<std::string> serialized_entries;
	for(const auto& [name, entry] : m_entries) {
		std::string serialized_entry = name + "," + std::to_string(entry.phone_number) + ","
		                               + std::to_string(static_cast<unsigned>(entry.birthday.month())) + "/"
		                               + std::to_string(static_cast<unsigned>(entry.birthday.day()));
		serialized_entries.push_back(serialized_entry);
	}
	std::vector<std::string> merged_entries = provider.synchronize(serialized_entries);
	m_entries.clear();
	for(const std::string& serialized_entry : merged_entries) {
		std::string name(max_name_length, '\0');
		std::uint64_t phone_number;
		unsigned month;
		unsigned day;
		char format[32] = {0};
		snprintf(format, sizeof(format), "%%%zu[^,],%%lu,%%u/%%u", max_name_length);
		sscanf(serialized_entry.c_str(), format, name.data(), &phone_number, &month, &day);
		name.erase(name.find('\0'));
		m_entries.try_emplace(normalize_name(name),
		    entry{phone_number, std::chrono::month_day{std::chrono::month{month}, std::chrono::day{day}}});
	}
}

address_book::entry& address_book::get_entry(std::string name) {
	auto it = m_entries.find(normalize_name(name));
	if(it == m_entries.end()) { throw std::invalid_argument("Entry not found"); }
	return it->second;
}

const address_book::entry& address_book::get_entry(std::string name) const {
	auto it = m_entries.find(normalize_name(name));
	if(it == m_entries.end()) { throw std::invalid_argument("Entry not found"); }
	return it->second;
}