#pragma once

#include <string>
#include <set>

namespace Copium
{
  enum class BindingType
  {
    Sampler2D, UniformBuffer
  };

  enum class ShaderType
  {
    Vertex, Fragment
  };

  struct ShaderBinding
  {
    std::string name;
    uint32_t set;
    uint32_t binding;
    uint32_t arraySize;
    BindingType bindingType;
    ShaderType shaderType;

    bool operator<(const ShaderBinding& rhs) const
    {
      if (set != rhs.set)
        return set < rhs.set;
      if (binding != rhs.binding)
        return binding < rhs.binding;
    }
  };

  class ShaderReflector
  {
  public:
    std::set<ShaderBinding> bindings;
  public:
    ShaderReflector(const std::string& vertexGlslFile, const std::string& fragmentGlslFile);

  private:
    void ParseGlslFile(const std::string& glslFile, ShaderType type);
    void ParseLine(const std::string& str, int& index);
    void ParseWhitespace(const std::string& str, int& index);
    void ParseLayout(const std::string& str, int& index, ShaderType type);
    std::string_view ParseWord(const std::string& str, int& index);
    void ParseUniformBuffer(const std::string& str, int& index);
  };
}
