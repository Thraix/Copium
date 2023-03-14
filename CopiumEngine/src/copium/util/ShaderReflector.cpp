#include "copium/util/ShaderReflector.h"

#include "copium/util/FileSystem.h"

#include <string_view>

namespace Copium
{

  ShaderReflector::ShaderReflector(const std::string& vertexGlslFile, const std::string& fragmentGlslFile)
  {
    ParseGlslFile(vertexGlslFile, ShaderType::Vertex);
    ParseGlslFile(fragmentGlslFile, ShaderType::Fragment);
  }

  void ShaderReflector::ParseGlslFile(const std::string& glslFile, ShaderType shaderType)
  {
    std::string str = FileSystem::ReadFileStr(glslFile);
    int index = 0;
    while (index < str.size())
    {
      ParseWhitespace(str, index);
      if (str[index] == '#')
      {
        ParseLine(str, index);
        continue;
      }
      if (std::string_view(&str[index], sizeof("layout") - 1) == "layout")
      {
        ParseLayout(str, index, shaderType);
        continue;
      }
      ParseLine(str, index);
    }
  }

  void ShaderReflector::ParseLine(const std::string& str, int& index)
  {
    while(str[index] != '\n' && index < str.size()) index++;
  }

  void ShaderReflector::ParseWhitespace(const std::string& str, int& index)
  {
    while ((str[index] == '\n' || str[index] == ' ' || str[index] == '\t' || str[index] == '\r') && index < str.size()) index++;
  }

  void ShaderReflector::ParseLayout(const std::string& str, int& index, ShaderType shaderType)
  {
    // TODO: Make more robust, currently we might get a crash on the string if the glsl file is invalid
    index += sizeof("layout") - 1;
    ParseWhitespace(str, index);
    index++; // "("
    ParseWhitespace(str, index);
    if (std::string_view(&str[index], sizeof("set") - 1) != "set")
    {
      ParseLine(str, index);
      return;
    }
    ShaderBinding shaderBinding;
    shaderBinding.shaderType = shaderType;
    index += sizeof("set") - 1;
    ParseWhitespace(str, index);
    index++; // "="
    ParseWhitespace(str, index);
    char* end;
    shaderBinding.set = std::strtol(&str[index], &end, 10);
    index = end - str.c_str();
    ParseWhitespace(str, index);
    index++; // ","
    ParseWhitespace(str, index);
    index += sizeof("binding") - 1;
    ParseWhitespace(str, index);
    index++; // "="
    ParseWhitespace(str, index);
    shaderBinding.binding = std::strtol(&str[index], &end, 10);
    index = end - str.c_str();
    ParseWhitespace(str, index);
    index++; // ")
    ParseWhitespace(str, index);
    index += sizeof("uniform") - 1;
    ParseWhitespace(str, index);

    std::string_view type = ParseWord(str, index);
    ParseWhitespace(str, index);
    if (str[index] == '{') ParseUniformBuffer(str, index);
    ParseWhitespace(str, index);
    std::string_view name = ParseWord(str, index);
    shaderBinding.name = name;
    ParseWhitespace(str, index);

    if (str[index] == '[')
    {
      index++;
      shaderBinding.arraySize = std::strtol(&str[index], &end, 10);
    }
    else
    {
      shaderBinding.arraySize = 1;
    }

    ParseLine(str, index);
    if (type == "sampler2D")
      shaderBinding.bindingType = BindingType::Sampler2D;
    else
      shaderBinding.bindingType = BindingType::UniformBuffer;
    CP_ASSERT(bindings.emplace(shaderBinding).second, "ParseLayout : multiple layouts with the same binding");
  }

  std::string_view ShaderReflector::ParseWord(const std::string& str, int& index)
  {
    int start = index;
    while (((str[index] >= 'a' && str[index] < 'z') || 
            (str[index] >= 'A' && str[index] < 'Z') || 
            (str[index] >= '0' && str[index] <= '9')) ||
             str[index] == '_' && 
           index < str.size()) index++;
    return std::string_view(&str[start], index - start);
  }

  void ShaderReflector::ParseUniformBuffer(const std::string& str, int& index) 
  {
    while (str[index] != '}' && index < str.size()) index++;
    if (index < str.size()) index++; // go past "}"
  }
}