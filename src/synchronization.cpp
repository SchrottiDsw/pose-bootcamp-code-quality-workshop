#include "synchronization.hpp"

#define HTTP_IMPLEMENTATION
#include <http.h>

#include <algorithm>
#include <fstream>
#include <sstream>

std::vector<std::string> synchronization_provider::merge_entries(
    std::vector<std::string> local_entries, std::vector<std::string> remote_entries) {
	std::vector<std::string> merged_entries = local_entries;
	for(std::string remote_entry : remote_entries) {
		bool found = false;
		std::string name = remote_entry.substr(0, remote_entry.find(','));
		if(std::none_of(merged_entries.begin(), merged_entries.end(),
		       [&name](std::string entry) { return entry.substr(0, entry.find(',')) == name; })) {
			merged_entries.push_back(remote_entry);
		}
	}
	return merged_entries;
}

http_synchronization_provider::http_synchronization_provider(std::string url) : m_url(url) {}

void await_request(http_t* request) {
	http_status_t status = HTTP_STATUS_PENDING;
	while(status == HTTP_STATUS_PENDING) {
		status = http_process(request);
	}
	if(status == HTTP_STATUS_FAILED) {
		printf("HTTP request failed (%d): %s.\n", request->status_code, request->reason_phrase);
		http_release(request);
		throw std::runtime_error("HTTP request failed");
	}
}

std::vector<std::string> http_synchronization_provider::synchronize(std::vector<std::string> serialized_entries) {
	http_t* get_request = http_get((m_url + "/address-book").c_str(), NULL);
	if(!get_request) { throw std::runtime_error("Invalid request"); }
	await_request(get_request);
	std::string response((const char*)get_request->response_data);
	http_release(get_request);

	std::vector<std::string> remote_entries;
	std::stringstream ss{response};
	std::string line;
	while(std::getline(ss, line)) {
		remote_entries.push_back(line);
	}

	std::vector<std::string> merged_entries = merge_entries(serialized_entries, remote_entries);

	std::string merged_entries_str;
	for(std::string entry : merged_entries) {
		merged_entries_str += entry + '\n';
	}

	http_t* post_request =
	    http_post((m_url + "/address-book").c_str(), merged_entries_str.c_str(), merged_entries_str.size(), NULL);
	if(!post_request) { throw std::runtime_error("Invalid request"); }
	await_request(post_request);
	http_release(post_request);

	return merged_entries;
}

file_synchronization_provider::file_synchronization_provider(std::string path) : m_path(path) {}

std::vector<std::string> file_synchronization_provider::synchronize(std::vector<std::string> serialized_entries) {
	std::vector<std::string> file_entries;
	{
		std::fstream file{m_path, std::ios::in | std::ios::binary};
		if(file.is_open()) {
			std::string line;
			while(std::getline(file, line)) {
				file_entries.push_back(line);
			}
		}
	}
	std::vector<std::string> merged_entries = merge_entries(serialized_entries, file_entries);
	{
		std::fstream file{m_path, std::ios::out | std::ios::binary | std::ios::trunc};
		if(!file.is_open()) { throw std::runtime_error("Could not open file " + m_path); }
		for(std::string entry : merged_entries) {
			file << entry << '\n';
		}
	}

	return merged_entries;
}