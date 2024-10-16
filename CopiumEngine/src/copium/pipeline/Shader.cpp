#include "Shader.h"

#include "copium/util/FileSystem.h"
#include "copium/core/Vulkan.h"
#include "copium/util/RuntimeException.h"

namespace Copium
{
  Shader::Shader(ShaderReadType type, const std::string& vertexInput, const std::string& fragmentInput)
  {
    switch (type)
    {
    case ShaderReadType::GlslCode:
      vertShaderModule = InitializeShaderModuleFromGlslCode(vertexInput, shaderc_vertex_shader);
      fragShaderModule = InitializeShaderModuleFromGlslCode(fragmentInput, shaderc_fragment_shader);
      break;
    case ShaderReadType::GlslFile:
      vertShaderModule = InitializeShaderModuleFromGlslFile(vertexInput, shaderc_vertex_shader);
      fragShaderModule = InitializeShaderModuleFromGlslFile(fragmentInput, shaderc_fragment_shader);
      break;
    case ShaderReadType::SpvCode:
      vertShaderModule = InitializeShaderModule(vertexInput);
      fragShaderModule = InitializeShaderModule(fragmentInput);
      break;
    case ShaderReadType::SpvFile:
      vertShaderModule = InitializeShaderModule(FileSystem::ReadFile(vertexInput));
      fragShaderModule = InitializeShaderModule(FileSystem::ReadFile(fragmentInput));
      break;
    default:
      CP_ASSERT(false, "Unreachable switch case: %s", ToString(type).c_str());
    }

    shaderStages.resize(2);
    shaderStages[0] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";

    shaderStages[1] = {};
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";
  }

  Shader::~Shader()
  {
    VkShaderModule vertShaderModuleCpy = vertShaderModule;
    VkShaderModule fragShaderModuleCpy = fragShaderModule;
    Vulkan::GetDevice().QueueIdleCommand([vertShaderModuleCpy, fragShaderModuleCpy]() {
      vkDestroyShaderModule(Vulkan::GetDevice(), vertShaderModuleCpy, nullptr);
      vkDestroyShaderModule(Vulkan::GetDevice(), fragShaderModuleCpy, nullptr);
    });
  }

  const std::vector<VkPipelineShaderStageCreateInfo> Shader::GetShaderStages() const
  {
    return shaderStages;
  }

  VkShaderModule Shader::InitializeShaderModule(const std::vector<uint32_t>& codeSpv)
  {
    return InitializeShaderModule(codeSpv.data(), codeSpv.size() * sizeof(uint32_t));
  }

  VkShaderModule Shader::InitializeShaderModule(const std::string& codeSpv)
  {
    return InitializeShaderModule(reinterpret_cast<const uint32_t*>(codeSpv.data()), codeSpv.size());
  }

  VkShaderModule Shader::InitializeShaderModule(const std::vector<char>& codeSpv)
  {
    return InitializeShaderModule(reinterpret_cast<const uint32_t*>(codeSpv.data()), codeSpv.size());
  }

  VkShaderModule Shader::InitializeShaderModuleFromGlslFile(const std::string& filename, shaderc_shader_kind type)
  {
    std::string spvFilename = ".cache/" + filename + ".spv";
    try
    {
      if (FileSystem::FileExists(spvFilename))
      {
        if (FileSystem::DateModified(filename) < FileSystem::DateModified(spvFilename))
        {
          CP_DEBUG("Loading cached shader file: %s", filename.c_str());
          std::vector<uint32_t> data = FileSystem::ReadFile32(spvFilename);
          return InitializeShaderModule(data.data(), data.size() * sizeof(uint32_t));
        }
      }
    }
    catch (const RuntimeException& e)
    {
      CP_WARN("Cached shader file is invalid, recreating it");
    }
    CP_DEBUG("Compiling shader file: %s", filename.c_str());
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_size);

    std::vector<char> glslCode = FileSystem::ReadFile(filename);
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(glslCode.data(), glslCode.size(), type, filename.c_str(), options);
    CP_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile shader: %s\n%s", filename.c_str(), result.GetErrorMessage().c_str());

    std::vector<uint32_t> data{result.cbegin(), result.cend()};
    FileSystem::WriteFile(spvFilename, (const char*)data.data(), data.size() * sizeof(uint32_t));
    return InitializeShaderModule(data.data(), data.size() * sizeof(uint32_t));
  }

  VkShaderModule Shader::InitializeShaderModuleFromGlslCode(const std::string& code, shaderc_shader_kind type)
  {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(code.data(), type, "inline_shader_code", options);
    CP_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile inline shader code: %s", result.GetErrorMessage());

    std::vector<uint32_t> data{result.cbegin(), result.cend()};
    return InitializeShaderModule(data.data(), data.size() * sizeof(uint32_t));
  }

  VkShaderModule Shader::InitializeShaderModule(const uint32_t* data, size_t size)
  {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = data;

    VkShaderModule shaderModule;
    CP_VK_ASSERT(vkCreateShaderModule(Vulkan::GetDevice(), &createInfo, nullptr, &shaderModule), "Failed to initialize shader module");

    return shaderModule;
  }
}
