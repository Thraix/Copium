#include "copium/util/FileSystem.h"

namespace Copium
{
  std::vector<char> FileSystem::ReadFile(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    CP_ASSERT(file.is_open(), "ReadFile : Failed to open file");

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
  }

  std::vector<uint32_t> FileSystem::ReadFile32(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    CP_ASSERT(file.is_open(), "ReadFile32 : Failed to open file");

    size_t fileSize = (size_t)file.tellg();
    CP_ASSERT(fileSize % 4 == 0, "ReadFile32 : byte size is not divisible by 4");
    std::vector<uint32_t> buffer(fileSize / 4);

    file.seekg(0);
    file.read((char*)buffer.data(), fileSize);

    return buffer;
  }

  std::string FileSystem::ReadFileStr(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    CP_ASSERT(file.is_open(), "ReadFileStr : Failed to open file");

    size_t fileSize = (size_t)file.tellg();
    std::string buffer;
    buffer.resize(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    return buffer;
  }

  void FileSystem::WriteFile(const std::string& filename, const std::string& data)
  {
    std::filesystem::path path{filename};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(filename, std::ios::binary);
    CP_ASSERT(file.is_open(), "WriteFile : Failed to open file");

    file.write(data.c_str(), data.size());
  }

  void FileSystem::WriteFile(const std::string& filename, const char* data, size_t size)
  {
    std::filesystem::path path{filename};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(filename, std::ios::binary);
    CP_ASSERT(file.is_open(), "WriteFile : Failed to open file");

    file.write(data, size);
  }

  bool FileSystem::FileExists(const std::string& filename)
  {
    std::ifstream file(filename);
    return file.good();
  }

  int64_t FileSystem::DateModified(const std::string& filename)
  {
    struct stat result;
    CP_ASSERT(stat(filename.c_str(), &result) == 0, "DataModified : Cannot stat file %s", filename.c_str());
    return (int64_t)result.st_mtime;
  }
}