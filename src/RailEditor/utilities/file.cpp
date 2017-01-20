#include "file.hpp"

#include <fstream>
#include <iostream>

std::unique_ptr<char[]> utilities::load_file_contents(const char* filename) {
	std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
	if (!ifs.is_open()) {
		std::cerr << "Cannot open file " << filename << ".\n";
	}

	std::size_t pos = ifs.tellg();

	std::unique_ptr<char[]> result(new char[pos + 1]);

	ifs.seekg(0, std::ios::beg);
	ifs.read(result.get(), pos);

	result[pos] = '\0';

	return result;
}

void utilities::save_file_contents(const char* filename, const char* contents, std::size_t size) {
	std::ofstream ofs(filename, std::ios::binary | std::ios::out);
	ofs.write(contents, size);
}
