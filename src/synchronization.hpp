#pragma once

#include <string>
#include <vector>

/// Abstract base class for synchronization providers.
class synchronization_provider {
  public:
	virtual std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) = 0;
	virtual ~synchronization_provider() = default;

  protected:
	std::vector<std::string> merge_entries(
	    std::vector<std::string> local_entries, std::vector<std::string> remote_entries);
};

// Synchronization provider that synchronizes with a HTTP server.
class http_synchronization_provider : public synchronization_provider {
  public:
	http_synchronization_provider(std::string url);
	std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override;

  private:
	std::string m_url;
};

// Synchronization provider that synchronizes with a file.
class file_synchronization_provider : public synchronization_provider {
  public:
	file_synchronization_provider(std::string path);
	std::vector<std::string> synchronize(std::vector<std::string> serialized_entries) override;

  private:
	std::string m_path;
};