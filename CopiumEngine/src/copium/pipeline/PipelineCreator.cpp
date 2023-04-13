#include "copium/pipeline/PipelineCreator.h"

#include "copium/util/Common.h"

namespace Copium
{
  PipelineCreator::PipelineCreator(VkRenderPass renderPass, const std::string& vertexShader, const std::string& fragmentShader)
    : vertexShader{vertexShader},
      fragmentShader{fragmentShader},
      shaderReflector{vertexShader, fragmentShader},
      renderPass{renderPass}
  {
    AddShaderDescription();
  }

  void PipelineCreator::SetVertexDescriptor(const VertexDescriptor& descriptor)
  {
    vertexDescriptor = descriptor;
  }

  void PipelineCreator::SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology)
  {
    topology = primitiveTopology;
  }

  void PipelineCreator::SetCullMode(VkCullModeFlags flags)
  {
    cullMode = flags;
  }

  void PipelineCreator::SetCullFrontFace(VkFrontFace cullFrontFace)
  {
    frontFace = cullFrontFace;
  }

  void PipelineCreator::SetDepthTest(bool depthTest)
  {
    this->depthTest = depthTest;
  }

  void PipelineCreator::AddShaderDescription()
  {
    for (auto& binding : shaderReflector.bindings)
    {
      descriptorSetLayouts[binding.set].emplace_back(DescriptorSetBinding{binding.binding, GetDescriptorType(binding.bindingType), binding.arraySize, GetShaderStageFlags(binding.shaderType)});
    }
  }

  VkDescriptorType PipelineCreator::GetDescriptorType(BindingType type)
  {
    switch (type)
    {
    case BindingType::Sampler2D:
      return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case BindingType::UniformBuffer:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    default:
      CP_ABORT("Unhandled switch case");
    }
  }

  VkShaderStageFlags PipelineCreator::GetShaderStageFlags(ShaderType type)
  {
    switch (type)
    {
    case ShaderType::Vertex:
      return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderType::Fragment:
      return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
      CP_ABORT("Unhandled switch case");
    }
  }
}