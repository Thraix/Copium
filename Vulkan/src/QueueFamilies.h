#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace Copium
{
  struct QueueFamiliesQuery
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    QueueFamiliesQuery(VkSurfaceKHR surface, VkPhysicalDevice device)
    {
      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

      int i = 0;
      for (auto&& queueFamily : queueFamilies)
      {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
          graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
          presentFamily = i;
        }
        i++;
      }
    }

    bool AllRequiredFamiliesSupported()
    {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };
}
