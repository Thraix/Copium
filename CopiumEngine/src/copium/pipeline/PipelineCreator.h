#pragma once

#include "copium/pipeline/ShaderReflector.h"
#include "copium/pipeline/VertexDescriptor.h"

#include <vulkan/vulkan.hpp>

#include <map>
#include <string>
#include <cstdint>

namespace Copium
{
  class PipelineCreator
  {
    struct DescriptorSetBinding
    {
      uint32_t binding;
      VkDescriptorType type;
      uint32_t count;
      VkShaderStageFlags flags;
    };
    friend class Pipeline;
  private:
    std::map<uint32_t, std::vector<DescriptorSetBinding>> descriptorSetLayouts{};

    std::string vertexShader;
    std::string fragmentShader;

    ShaderReflector shaderReflector;
    VertexDescriptor vertexDescriptor{};
    VkPrimitiveTopology topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
    VkCullModeFlags cullMode{VK_CULL_MODE_FRONT_BIT};
    VkFrontFace frontFace{VK_FRONT_FACE_CLOCKWISE};
    VkRenderPass renderPass{VK_NULL_HANDLE};
    bool depthTest{true};
    bool blending{false};

  public:
    PipelineCreator(VkRenderPass renderPass, const std::string& vertexShader, const std::string& fragmentShader);

    void SetVertexDescriptor(const VertexDescriptor& descriptor);
    void SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology);
    void SetCullMode(VkCullModeFlags flags);
    void SetCullFrontFace(VkFrontFace cullFrontFace);
    void SetDepthTest(bool depthTest);
    void SetBlending(bool blending);
  private:
    void AddShaderDescription();
    static VkDescriptorType GetDescriptorType(BindingType type);
    static VkShaderStageFlags GetShaderStageFlags(ShaderType type);
  };
}
