#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace Copium
{
  class MetaFileClass
  {
    friend class MetaFile;
    private:
      std::map<std::string, std::string> values;

    public:
      MetaFileClass() {}
      MetaFileClass(const std::map<std::string, std::string>& values)
        : values{values}
      {}
      std::string GetValue(const std::string& key, const std::string& val) const;
      bool HasValue(const std::string& key) const;
      const std::string& GetValue(const std::string& key) const;
      const std::map<std::string, std::string>& GetValues() const;

      void AddValue(const std::string& key, const std::string& val);

      friend std::ostream& operator<<(std::ostream& stream, const MetaFileClass& file);
  };

  class MetaFile
  {
    private:
      std::string filepath;
      std::map<std::string, MetaFileClass> classes; // map<class, map<variable, value>>

    public:
      MetaFile();
      MetaFile(const std::string& filepath);
      MetaFile(std::istream& stream);

      bool HasMetaClass(const std::string& className) const;

      const std::map<std::string, MetaFileClass>& GetMetaFileClasses();
      MetaFileClass& GetMetaClass(const std::string& className);
      const MetaFileClass& GetMetaClass(const std::string& className) const;
      const std::string& GetFilePath()  const;

      void AddMetaClass(const std::string& name, const MetaFileClass& metaClass);

      friend std::ostream& operator<<(std::ostream& stream, const MetaFile& file);
      friend std::istream& operator>>(std::istream& stream, MetaFile& file);

      static std::vector<MetaFile> ReadList(const std::string& file);
    private:
      void LoadMetaFile(std::istream& stream);
  };
}
