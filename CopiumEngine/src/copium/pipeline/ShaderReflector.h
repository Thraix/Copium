#pragma once

#include "copium/pipeline/ShaderBinding.h"

#include <set>
#include <string>

namespace Copium
{
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
    void ParseUniformBuffer(const std::string& str, int& index, ShaderBinding& binding);
  };
}
