#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class VertexDescriptor
  {
  private:
    uint32_t bindingIndex = 0;
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

  public:
    void AddAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset, uint32_t size);
    VkDeviceSize GetVertexSize() const;
    const std::vector<VkVertexInputAttributeDescription>& GetAttributes() const;
    const std::vector<VkVertexInputBindingDescription>& GetBindings() const;

  private:
    uint32_t AddLayout(uint32_t binding, uint32_t size);
  };
}
