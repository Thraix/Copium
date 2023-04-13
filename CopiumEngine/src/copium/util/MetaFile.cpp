#include "copium/util/MetaFile.h"

#include "copium/util/Common.h"
#include "copium/util/StringUtil.h"

#include <fstream>

namespace Copium
{
  std::string MetaFileClass::GetValue(const std::string& key, const std::string& val) const
  {
    auto it = values.find(key);
    if(it != values.end())
      return it->second;
    return val;
  }

  bool MetaFileClass::HasValue(const std::string& key) const
  {
    return values.find(key) != values.end();
  }

  const std::string& MetaFileClass::GetValue(const std::string& key) const
  {
    auto it = values.find(key);
    CP_ASSERT(it != values.end(), "GetValue : Value does not exist: %s", key.c_str());
    return it->second;
  }

  const std::map<std::string, std::string>& MetaFileClass::GetValues() const
  {
    return values;
  }

  void MetaFileClass::AddValue(const std::string& key, const std::string& val)
  {
    values.emplace(key, val);
  }

  std::ostream& operator<<(std::ostream& stream, const MetaFileClass& file)
  {
    for(auto value : file.GetValues())
    {
      stream << value.first << "=" << value.second << std::endl;
    }
    return stream;
  }

  MetaFile::MetaFile() {}
  MetaFile::MetaFile(const std::string& filepath)
    : filepath{filepath}
  {
    std::ifstream stream(filepath);

    CP_ASSERT(stream.is_open(), "MetaFile : Could not find meta file: %s", filepath.c_str());
    LoadMetaFile(stream);
  }

  MetaFile::MetaFile(std::istream& stream)
  {
    LoadMetaFile(stream);
  }

  bool MetaFile::HasMetaClass(const std::string& className) const
  {
    return classes.find(className) != classes.end();
  }

  MetaFileClass& MetaFile::GetMetaClass(const std::string& className)
  {
    auto it = classes.find(className);
    CP_ASSERT(it != classes.end(), "GetMetaClass : class does not exist: %s", className.c_str());
    return it->second;
  }

  const MetaFileClass& MetaFile::GetMetaClass(const std::string& className) const
  {
    auto it = classes.find(className);
    CP_ASSERT(it != classes.end(), "GetMetaClass : class does not exist: ", className.c_str());
    return it->second;
  }

  const std::string& MetaFile::GetFilePath()  const
  {
    return filepath;
  }

  void MetaFile::AddMetaClass(const std::string& name, const MetaFileClass& metaClass)
  {
    classes[name] = metaClass;
  }

  std::ostream& operator<<(std::ostream& stream, const MetaFile& file)
  {
    for(auto metaClass : file.classes)
    {
      stream << "[" << metaClass.first << "]" << std::endl;
      stream << metaClass.second;
    }
    return stream;
  }

  std::istream& operator>>(std::istream& stream, MetaFile& file)
  {
    file.classes.clear();
    file.LoadMetaFile(stream);
    return stream;
  }

  void MetaFile::LoadMetaFile(std::istream& stream)
  {
    std::string currentClass = "";
    auto metaClassIt = classes.end();
    std::string line;
    while(std::getline(stream, line))
    {
      std::string_view trimmedLine = StringUtil::Trim(line);
      if(trimmedLine.empty())
        continue;

      if(trimmedLine == "---")
      {
        return;
      }

      if(trimmedLine.front() == '[' && trimmedLine.back() == ']' )
      {
        currentClass = StringUtil::Trim(line);
        currentClass = currentClass.substr(1, currentClass.size() - 2);
        metaClassIt = classes.find(currentClass);
        CP_ASSERT(metaClassIt == classes.end(), "LoadMetaFile : Meta file contains two of the same class: %s", currentClass.c_str());
        metaClassIt = classes.emplace(currentClass, MetaFileClass{}).first;
        continue;
      }

      size_t pos = line.find("=");
      if(pos == std::string::npos)
      {
        CP_WARN("LoadMetaFile : Meta file line does not contain \'=\'");
        continue;
      }

      std::string_view key = StringUtil::Trim(std::string_view(line.c_str(), pos));
      std::string_view value = StringUtil::Trim(std::string_view(line.c_str() + pos + 1));
      if(key.length() == 0)
      {
        CP_WARN("LoadMetaFile : MetaFile key is empty");
        continue;
      }

      CP_ASSERT(metaClassIt != classes.end(), "LoadMetaFile : No meta file header specified: ", filepath.c_str());
      auto res = metaClassIt->second.values.emplace(key, value);
      if(!res.second)
      {
        CP_WARN("LoadMetaFile : Meta file key is defined twice: %s", std::string(key).c_str());
      }
    }
  }

  std::vector<MetaFile> MetaFile::ReadList(const std::string& file)
  {
    std::vector<MetaFile> metaFiles;
    std::ifstream stream{file};
    if(stream)
    {
      MetaFile meta;
      while(!stream.eof())
      {
        MetaFile meta{};
        stream >> meta;
        if(meta.classes.empty())
          continue;
        metaFiles.emplace_back(meta);
      }
      return metaFiles;
    }
    return {};
  }
}
