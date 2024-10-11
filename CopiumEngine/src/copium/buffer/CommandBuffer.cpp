#include "copium/buffer/CommandBuffer.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  CommandBuffer::CommandBuffer(CommandBufferType type)
    : type{type}
  {
    switch (type)
    {
    case CommandBufferType::SingleUse:
      commandBuffers.resize(1);
      break;
    case CommandBufferType::Dynamic:
      commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
      break;
    default:
      CP_ABORT("Unreachable switch case: %s", ToString(type).c_str());
    }

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = Vulkan::GetDevice().GetCommandPool();
    allocateInfo.commandBufferCount = commandBuffers.size();
    CP_VK_ASSERT(vkAllocateCommandBuffers(Vulkan::GetDevice(), &allocateInfo, commandBuffers.data()), "Failed to allocate CommandBuffer");
  }

  CommandBuffer::~CommandBuffer()
  {
    std::vector<VkCommandBuffer> commandBuffersCpy = commandBuffers;
    Vulkan::GetDevice().QueueIdleCommand([commandBuffersCpy]() {
      vkFreeCommandBuffers(Vulkan::GetDevice(), Vulkan::GetDevice().GetCommandPool(), commandBuffersCpy.size(), commandBuffersCpy.data());
    });
  }

  // TODO: Test as constexpr function to see if it avoids the switch case
  void CommandBuffer::Begin()
  {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    switch (type)
    {
    case CommandBufferType::SingleUse:
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      break;
    case CommandBufferType::Dynamic:
      break;
    default:
      CP_ABORT("Unreachable switch case: %s", ToString(type).c_str());
    }

    vkResetCommandBuffer(commandBuffers[Vulkan::GetSwapChain().GetFlightIndex()], 0);
    CP_VK_ASSERT(vkBeginCommandBuffer(commandBuffers[Vulkan::GetSwapChain().GetFlightIndex()], &beginInfo), "Failed to begin command buffer");
  }

  void CommandBuffer::End()
  {
    vkEndCommandBuffer(commandBuffers[Vulkan::GetSwapChain().GetFlightIndex()]);
  }

  void CommandBuffer::Submit()
  {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[Vulkan::GetSwapChain().GetFlightIndex()];

    vkQueueSubmit(Vulkan::GetDevice().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    // TODO: if singleUse?
    vkQueueWaitIdle(Vulkan::GetDevice().GetGraphicsQueue());
  }

  CommandBuffer::operator VkCommandBuffer() const
  {
    return commandBuffers[Vulkan::GetSwapChain().GetFlightIndex()];
  }
}