#pragma once

#include <sys/types.h>
#include <sys/stat.h>

#include "Common.h"

#include <fstream>

namespace FileSystem
{
	static std::vector<char> ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		CP_ASSERT(file.is_open(), "Failed to open file");

		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		return buffer;
	}

	static std::string ReadFileStr(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		CP_ASSERT(file.is_open(), "Failed to open file");

		size_t fileSize = (size_t) file.tellg();
		std::string buffer;
		buffer.resize(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		return buffer;
	}

	static void WriteFile(const std::string& filename, const std::string& data)
	{
		std::ofstream file(filename, std::ios::binary);
		CP_ASSERT(file.is_open(), "Failed to open file");

		file.write(data.c_str(), data.size());
	}

	static void WriteFile(const std::string& filename, const char* data, size_t size)
	{
		std::ofstream file(filename, std::ios::binary);
		CP_ASSERT(file.is_open(), "Failed to open file");

		file.write(data, size);
	}

	static bool FileExists(const std::string& filename)
	{
		std::ifstream file(filename);
		return file.good();
	}

	static int64_t DateModified(const std::string& filename)
	{
		struct stat result;
		CP_ASSERT(stat(filename.c_str(), &result) == 0, "Cannot stat file %s", filename.c_str());
    return (int64_t)result.st_mtime;
	}
}