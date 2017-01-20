#pragma once

#include <memory>

namespace utilities {
	std::unique_ptr<char[]> load_file_contents(const char * filename);
	void save_file_contents(const char * filename, const char * contents, std::size_t size);
}
