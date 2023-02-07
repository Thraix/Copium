#include "VertexDescriptor.h"

#include "Common.h"

namespace Copium
{
  void VertexDescriptor::AddAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset, uint32_t size)
  {
    CP_ASSERT(binding <= bindings.size(), "AddAttribute : Attribute binding must less than or be equal to the amount of current bindings");

    if (binding == bindings.size())
      AddLayout(binding, size);

    VkVertexInputAttributeDescription description{};
    description.binding = binding;
    description.location = location;
    description.format = format;
    description.offset = offset;
    attributes.emplace_back(description);
  }

  VkDeviceSize VertexDescriptor::GetVertexSize() const
  {
    VkDeviceSize bufferSize = 0;
    for (auto&& binding : bindings)
    {
      bufferSize += binding.stride;
    }
    return bufferSize;
  }

  const std::vector<VkVertexInputAttributeDescription>& VertexDescriptor::GetAttributes() const
  {
    return attributes;
  }

  const std::vector<VkVertexInputBindingDescription>& VertexDescriptor::GetBindings() const
  {
    return bindings;
  }

  uint32_t VertexDescriptor::AddLayout(uint32_t binding, uint32_t size)
  {
    VkVertexInputBindingDescription description{};
    description.binding = binding;
    description.stride = size;
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(description);
    return description.binding;
  }
}