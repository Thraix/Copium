#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/core/Vulkan.h"
#include "copium/pipeline/ShaderBinding.h"
#include "copium/util/Common.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class UniformBuffer final : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(UniformBuffer);

    ShaderBinding binding;
    std::vector<uint8_t> buffer;

  public:
    UniformBuffer(Vulkan& vulkan, ShaderBinding binding);

    VkDescriptorBufferInfo GetDescriptorBufferInfo(int index) const;

    void Set(const std::string& str, const glm::mat3& data);
    void Set(const std::string& str, const glm::mat4& data);
    void Set(const std::string& str, const glm::vec2& data);
    void Set(const std::string& str, const glm::vec3& data);
    void Set(const std::string& str, const glm::vec4& data);
    void Set(const std::string& str, float data);
    void Set(const std::string& str, int data);

    void Update();
  };
}
