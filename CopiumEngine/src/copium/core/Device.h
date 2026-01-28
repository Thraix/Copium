#pragma once

#include <functional>
#include <queue>
#include <vulkan/vulkan.hpp>

#include "copium/util/Common.h"

namespace Copium
{
  class Vulkan;

  class Device
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Device);

  public:
    Device();
    ~Device();

    uint32_t GetGraphicsQueueFamily() const;
    uint32_t GetPresentQueueFamily() const;
    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;
    VkCommandPool GetCommandPool() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    operator VkDevice() const;
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void WaitIdle();
    void WaitIdleIfCommandQueued();
    void CleanupIdleQueue();
    void QueueIdleCommand(std::function<void()> idleCommand);

  private:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;

    // TODO: Move to SwapChain?
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    std::queue<std::function<void()>> idleCommands;
    // TODO end

  private:
    void SelectPhysicalDevice();
    void InitializeLogicalDevice();
    void InitializeCommandPool();
    uint32_t GetPhysicalDevicePriority(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredDeviceExtensions();
  };
}
