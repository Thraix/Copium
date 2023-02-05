#pragma once

#include "Common.h"
#include "Instance.h"
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBuffer);
  public:
    enum class Type
    {
      SingleUse, Dynamic
    };
  private:
    Instance& instance;

    std::vector<VkCommandBuffer> commandBuffers;
    const Type type;
    VkCommandBuffer currentCommandBuffer{VK_NULL_HANDLE};

  public:
    CommandBuffer(Instance& instance, Type type)
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

    ~CommandBuffer()
    {
      vkFreeCommandBuffers(instance.GetDevice(), instance.GetCommandPool(), commandBuffers.size(), commandBuffers.data());
    }

    operator VkCommandBuffer() const
    {
      return currentCommandBuffer;
    }

    // TODO: Test as constexpr function to see if it avoids the switch case
    void Begin()
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

    void End()
    {
      vkEndCommandBuffer(currentCommandBuffer);
    }

    void Submit()
    {
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &currentCommandBuffer;

      vkQueueSubmit(instance.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
      // TODO: if singleUse?
      vkQueueWaitIdle(instance.GetGraphicsQueue());
    }

    void SubmitAsGraphicsQueue()
    {
      instance.SubmitGraphicsQueue({currentCommandBuffer});
    }

    VkCommandBuffer GetHandle() const
    {
      return currentCommandBuffer;
    }
  };
}
