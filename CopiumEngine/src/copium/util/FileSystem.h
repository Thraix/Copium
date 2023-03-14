#pragma once

#include "copium/util/Common.h"

#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

namespace Copium
{
	class FileSystem
	{
		CP_STATIC_CLASS(FileSystem);
	public:
		static std::vector<char> ReadFile(const std::string& filename);
		static std::vector<uint32_t> ReadFile32(const std::string& filename);
		static std::string ReadFileStr(const std::string& filename);
		static void WriteFile(const std::string& filename, const std::string& data);
		static void WriteFile(const std::string& filename, const char* data, size_t size);
		static bool FileExists(const std::string& filename);
		static int64_t DateModified(const std::string& filename);
	};
}