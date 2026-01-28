#pragma once

#include <vulkan/vulkan.h>

#include <optional>

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
