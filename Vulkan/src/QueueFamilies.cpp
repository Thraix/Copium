#include "QueueFamilies.h"

#include <vector>

namespace Copium
{
  QueueFamiliesQuery::QueueFamiliesQuery(VkSurfaceKHR surface, VkPhysicalDevice device)
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

  bool QueueFamiliesQuery::AllRequiredFamiliesSupported()
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
}