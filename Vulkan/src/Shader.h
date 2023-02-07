#pragma once

#include <shaderc/shaderc.hpp>

#include "Common.h"
#include "Instance.h"

namespace Copium
{
  class Shader final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Shader);
  public:
    enum class Type
    {
      GlslFile, GlslCode, SpvFile, SpvCode
    };

  private:
    Instance& instance;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  public:
    Shader(Instance& instance, Type type, const std::string& vertexInput, const std::string& fragmentInput);
    ~Shader();

    const std::vector<VkPipelineShaderStageCreateInfo> GetShaderStages() const;
  private:
    VkShaderModule InitializeShaderModule(const std::vector<uint32_t>& codeSpv);
    VkShaderModule InitializeShaderModule(const std::string& codeSpv);
    VkShaderModule InitializeShaderModule(const std::vector<char>& codeSpv);
    VkShaderModule InitializeShaderModuleFromGlslFile(const std::string& filename, shaderc_shader_kind type);
    VkShaderModule InitializeShaderModuleFromGlslCode(const std::string& code, shaderc_shader_kind type);
    VkShaderModule InitializeShaderModule(const uint32_t* data, size_t size);
  };
}
