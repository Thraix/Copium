#include "copium/buffer/CommandBuffer.h"

#include "copium/core/Device.h"
#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"

namespace Copium
{
  CommandBuffer::CommandBuffer(Vulkan& vulkan, Type type)
    : vulkan{vulkan}, type{type}
  {
    switch (type)
    {
    case Type::SingleUse:
      commandBuffers.resize(1);
      break;
    case Type::Dynamic:
      commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
      break;
    default:
      CP_ABORT("CommandBuffer : Unreachable switch case");
    }

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = vulkan.GetDevice().GetCommandPool();
    allocateInfo.commandBufferCount = commandBuffers.size();
    CP_VK_ASSERT(vkAllocateCommandBuffers(vulkan.GetDevice(), &allocateInfo, commandBuffers.data()), "CommandBuffer : Failed to allocate CommandBuffer");
  }

  CommandBuffer::~CommandBuffer()
  {
    vkFreeCommandBuffers(vulkan.GetDevice(), vulkan.GetDevice().GetCommandPool(), commandBuffers.size(), commandBuffers.data());
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
    case Type::SingleUse:
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
      break;
    case Type::Dynamic:
      break;
    default:
      CP_ABORT("Begin : Unreachable switch case");
    }

    vkResetCommandBuffer(commandBuffers[vulkan.GetSwapChain().GetFlightIndex()], 0);
    CP_VK_ASSERT(vkBeginCommandBuffer(commandBuffers[vulkan.GetSwapChain().GetFlightIndex()], &beginInfo), "Begin : Failed to begin command buffer");
  }

  void CommandBuffer::End()
  {
    vkEndCommandBuffer(commandBuffers[vulkan.GetSwapChain().GetFlightIndex()]);
  }

  void CommandBuffer::Submit()
  {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[vulkan.GetSwapChain().GetFlightIndex()];

    vkQueueSubmit(vulkan.GetDevice().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    // TODO: if singleUse?
    vkQueueWaitIdle(vulkan.GetDevice().GetGraphicsQueue());
  }

  CommandBuffer::operator VkCommandBuffer() const
  {
    return commandBuffers[vulkan.GetSwapChain().GetFlightIndex()];
  }
}