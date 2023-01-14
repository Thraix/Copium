#pragma once

#include <map>
#include <vulkan/vulkan.hpp>

class VertexDescriptor
{
private:
  uint32_t bindingIndex = 0;
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;

public:
  template <typename T>
  void AddAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
  {
    auto it = std::find_if(bindings.begin(), bindings.end(), [&binding](const VkVertexInputBindingDescription& description) { return description.binding == binding; });
    if (it == bindings.end())
      AddLayout(binding, sizeof(T));

    VkVertexInputAttributeDescription description{};
    description.binding = binding;
    description.location = location;
    description.format = format;
    description.offset = offset;
    attributes.emplace_back(description);
  }

  VkDeviceSize GetVertexSize() const
  {
    VkDeviceSize bufferSize = 0;
    for (auto&& binding : bindings)
    {
      bufferSize += binding.stride;
    }
    return bufferSize;
  }

private:
  uint32_t AddLayout(uint32_t binding, uint32_t size)
  {
    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = size;
    description.inputRate =  VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(description);
    return description.binding;
  }
};
