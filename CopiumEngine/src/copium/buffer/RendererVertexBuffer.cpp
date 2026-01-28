#include "copium/buffer/RendererVertexBuffer.h"

#include "copium/core/Vulkan.h"

namespace Copium
{

  RendererVertexBuffer::RendererVertexBuffer(const VertexDescriptor& descriptor, int vertexCount)
    : Buffer{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
             descriptor.GetVertexSize() * vertexCount,
             SwapChain::MAX_FRAMES_IN_FLIGHT}
  {
  }

  void RendererVertexBuffer::Bind(const CommandBuffer& commandBuffer)
  {
    VkDeviceSize offset = GetPosition(Vulkan::GetSwapChain().GetFlightIndex());
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &handle, &offset);
  }
}
