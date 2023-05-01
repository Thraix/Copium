#pragma once

#include "copium/util/Common.h"
#include "copium/util/Enum.h"

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.hpp>

#define CP_SHADER_READ_TYPE_ENUMS GlslFile, GlslCode, SpvFile, SpvCode
CP_ENUM_CREATOR(Copium, ShaderReadType, CP_SHADER_READ_TYPE_ENUMS);

namespace Copium
{
  class Shader final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Shader);
  private:
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  public:
    Shader(ShaderReadType type, const std::string& vertexInput, const std::string& fragmentInput);
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
