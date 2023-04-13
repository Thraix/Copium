#include "copium/buffer/UniformBuffer.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  UniformBuffer::UniformBuffer(ShaderBinding binding)
    : Buffer{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.GetUniformBufferSize(), SwapChain::MAX_FRAMES_IN_FLIGHT}, binding{binding}
  {
    buffer.resize(Buffer::GetSize());
  }

  VkDescriptorBufferInfo UniformBuffer::GetDescriptorBufferInfo(int index) const
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = handle;
    bufferInfo.offset = (VkDeviceSize)index * size;
    bufferInfo.range = size;
    return bufferInfo;
  }

  void UniformBuffer::Set(const std::string& str, const glm::mat3& data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Mat3, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(glm::mat3));
  }

  void UniformBuffer::Set(const std::string& str, const glm::mat4& data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Mat4, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(glm::mat4));
  }

  void UniformBuffer::Set(const std::string& str, const glm::vec2& data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Vec2, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(glm::vec2));
  }

  void UniformBuffer::Set(const std::string& str, const glm::vec3& data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Vec3, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(glm::vec3));
  }

  void UniformBuffer::Set(const std::string& str, const glm::vec4& data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Vec4, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(glm::vec4));
  }

  void UniformBuffer::Set(const std::string& str, float data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Float, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(float));
  }

  void UniformBuffer::Set(const std::string& str, int data)
  {
    CP_ASSERT(binding.GetUniformType(str) == UniformType::Int, "Uniform type missmatch = %s", str.c_str());
    uint32_t offset = binding.GetUniformOffset(str);
    memcpy(buffer.data() + offset, &data, sizeof(int));
  }

  void UniformBuffer::Update()
  {
    Buffer::Update(buffer.data(), Vulkan::GetSwapChain().GetFlightIndex());
  }
}