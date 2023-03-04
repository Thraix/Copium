#pragma once

#include "copium/core/QueueFamilies.h"
#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Vulkan;
  class Device
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Device);
  private:
    Vulkan& vulkan;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;

    // TODO: Move to SwapChain?
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    // TODO end

  public:
    Device(Vulkan& vulkan);
    ~Device();
    
    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;
    VkCommandPool GetCommandPool() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    operator VkDevice() const;
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  
  private:
    void SelectPhysicalDevice();
    void InitializeLogicalDevice();
    void InitializeCommandPool();
    bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredDeviceExtensions();
  };
}