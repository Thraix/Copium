#pragma once

#include <map>
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
    template <typename T>
    void AddAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
      CP_ASSERT(binding <= bindings.size(), "AddAttribute : Attribute binding must less than or be equal to the amount of current bindings");

      if (binding == bindings.size())
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

    const std::vector<VkVertexInputAttributeDescription>& GetAttributes() const
    {
      return attributes;
    }

    const std::vector<VkVertexInputBindingDescription>& GetBindings() const
    {
      return bindings;
    }

  private:
    uint32_t AddLayout(uint32_t binding, uint32_t size)
    {
      VkVertexInputBindingDescription description{};
      description.binding = binding;
      description.stride = size;
      description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
      bindings.emplace_back(description);
      return description.binding;
    }
  };
}
