#include "copium/buffer/CommandBuffer.h"

namespace Copium
{
  CommandBuffer::CommandBuffer(Instance& instance, Type type)
    : instance{instance}, type{type}
  {
    if (type == Type::Dynamic)
      commandBuffers.resize(instance.GetMaxFramesInFlight());
    else
      commandBuffers.resize(1);

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = instance.GetCommandPool();
    allocateInfo.commandBufferCount = commandBuffers.size();
    CP_VK_ASSERT(vkAllocateCommandBuffers(instance.GetDevice(), &allocateInfo, commandBuffers.data()), "CommandBuffer : Failed to allocate CommandBuffer");
  }

  CommandBuffer::~CommandBuffer()
  {
    vkFreeCommandBuffers(instance.GetDevice(), instance.GetCommandPool(), commandBuffers.size(), commandBuffers.data());
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
      currentCommandBuffer = commandBuffers.front();
      break;
    case Type::Dynamic:
      currentCommandBuffer = commandBuffers[instance.GetFlightIndex()];
      break;
    default:
      CP_ABORT("Begin : Unreachable switch case");
    }

    vkResetCommandBuffer(currentCommandBuffer, 0);
    CP_VK_ASSERT(vkBeginCommandBuffer(currentCommandBuffer, &beginInfo), "Begin : Failed to begin command buffer");
  }

  void CommandBuffer::End()
  {
    vkEndCommandBuffer(currentCommandBuffer);
  }

  void CommandBuffer::Submit()
  {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;

    vkQueueSubmit(instance.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    // TODO: if singleUse?
    vkQueueWaitIdle(instance.GetGraphicsQueue());
  }

  void CommandBuffer::SubmitAsGraphicsQueue()
  {
    instance.SubmitGraphicsQueue({currentCommandBuffer});
  }

  CommandBuffer::operator VkCommandBuffer() const
  {
    return currentCommandBuffer;
  }
}