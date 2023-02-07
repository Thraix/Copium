#include "copium/buffer/VertexBuffer.h"

namespace Copium
{
  VertexBuffer::VertexBuffer(Instance& instance, const VertexDescriptor& descriptor, int vertexCount)
    : Buffer{instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, descriptor.GetVertexSize() * vertexCount, 1}
  {
    VkDeviceSize offset = 0;
    for (auto&& binding : descriptor.GetBindings())
    {
      bindingOffsets.emplace_back(offset);
      bindingSizes.emplace_back(binding.stride * vertexCount);
      offset += binding.stride * vertexCount;
    }
  }

  void VertexBuffer::Bind(const CommandBuffer& commandBuffer)
  {
    std::vector<VkBuffer> buffers{bindingOffsets.size(), handle};
    vkCmdBindVertexBuffers(commandBuffer, 0, bindingOffsets.size(), buffers.data(), bindingOffsets.data());
  }

  void VertexBuffer::Update(uint32_t binding, void* data)
  {
    UpdateStaging(data, bindingOffsets[binding], bindingSizes[binding]);
  }
}