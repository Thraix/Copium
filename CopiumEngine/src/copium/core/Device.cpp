#include "Device.h"

#include "copium/core/Vulkan.h"
#include "copium/core/Window.h"

namespace Copium
{
  Device::Device()
  {
    SelectPhysicalDevice();
    InitializeLogicalDevice();
    InitializeCommandPool();
  }
  Device::~Device()
  {
    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);
  }

  uint32_t Device::GetGraphicsQueueFamily() const
  {
    return graphicsQueueIndex;
  }

  uint32_t Device::GetPresentQueueFamily() const
  {
    return presentQueueIndex;
  }

  VkQueue Device::GetGraphicsQueue() const
  {
    return graphicsQueue;
  }

  VkQueue Device::GetPresentQueue() const 
  {
    return presentQueue;
  }

  VkCommandPool Device::GetCommandPool() const
  {
    return commandPool;
  }

  VkPhysicalDevice Device::GetPhysicalDevice() const
  {
    return physicalDevice;
  }

  Device::operator VkDevice() const
  {
    return device;
  }

  uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
  {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
      if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        return i;
    }
    CP_ABORT("Failed to find suitable memory type");
  }

  void Device::SelectPhysicalDevice()
  {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(Vulkan::GetInstance(), &deviceCount, nullptr);
    CP_ASSERT(deviceCount != 0, "No available devices support Vulkan");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(Vulkan::GetInstance(), &deviceCount, devices.data());

    std::vector<std::pair<VkPhysicalDevice, uint32_t>> devicePriorities;
    devicePriorities.reserve(deviceCount);
    CP_INFO("Available devices:");
    for (auto&& device : devices)
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      CP_INFO_CONT("\t%s", deviceProperties.deviceName);
      devicePriorities.emplace_back(device, GetPhysicalDevicePriority(device));
    }

    std::sort(devicePriorities.begin(), devicePriorities.end(), [](const std::pair<VkPhysicalDevice, uint32_t>& lhs, const std::pair<VkPhysicalDevice, uint32_t>& rhs) { return lhs.second > rhs.second; });
    auto&& it = devicePriorities.begin();
    CP_ASSERT(it->second != 0, "Failed to find suitable gpu");

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(it->first, &deviceProperties);
    physicalDevice = it->first;
    CP_INFO("Selecting device: %s", deviceProperties.deviceName);
  }

  void Device::InitializeLogicalDevice()
  {
    QueueFamiliesQuery query{Vulkan::GetWindow().GetSurface(), physicalDevice};

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies{query.graphicsFamily.value(), query.presentFamily.value()};
    for (auto&& queueFamily : uniqueQueueFamilies)
    {
      VkDeviceQueueCreateInfo queueCreateInfo{};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;
      queueCreateInfo.queueCount = 1;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.emplace_back(queueCreateInfo);
    }
    std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledExtensionCount = deviceExtensions.size();

    CP_VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to initialize logical device");

    graphicsQueueIndex = query.graphicsFamily.value();
    presentQueueIndex = query.presentFamily.value();
    vkGetDeviceQueue(device, graphicsQueueIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueIndex, 0, &presentQueue);
  }

  void Device::InitializeCommandPool()
  {
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = graphicsQueueIndex;
    CP_VK_ASSERT(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool), "Failed to initialize command pool");
  }

  uint32_t Device::GetPhysicalDevicePriority(VkPhysicalDevice device)
  {
    uint32_t priority = 0;
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      priority = 100;
    else
      priority = 50;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    if (!deviceFeatures.fillModeNonSolid || !deviceFeatures.samplerAnisotropy)
      return 0;

    QueueFamiliesQuery query{Vulkan::GetWindow().GetSurface(), device};
    if (!query.AllRequiredFamiliesSupported())
      return 0;

    if (!CheckDeviceExtensionSupport(device))
      return 0;

    SwapChainSupportDetails details{Vulkan::GetWindow().GetSurface(), device};
    if (!details.Valid())
      return 0;

    return priority;
  }

  bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device)
  {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions{extensionCount};
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

    for (auto&& requiredExtension : GetRequiredDeviceExtensions())
    {
      bool found = false;
      for (auto&& extension : extensions)
      {
        if (std::strcmp(requiredExtension, extension.extensionName) == 0)
        {
          found = true;
          break;
        }
      }
      if (!found)
        return false;
    }
    return true;
  }

  std::vector<const char*> Device::GetRequiredDeviceExtensions()
  {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  }
}
