#pragma once

#include <optional>
#include <vulkan/vulkan.h>

namespace Copium
{
  struct QueueFamiliesQuery
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    QueueFamiliesQuery(VkSurfaceKHR surface, VkPhysicalDevice device);

    bool AllRequiredFamiliesSupported();
  };
}
