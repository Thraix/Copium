#include "Device.h"

#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"
#include "copium/core/Window.h"

namespace Copium
{
  Device::Device(Vulkan& vulkan)
    : vulkan{vulkan}
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
    CP_ABORT("FindMemoryType : Failed to find suitable memory type");
  }

  void Device::SelectPhysicalDevice()
  {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(vulkan.GetInstance(), &deviceCount, nullptr);
    CP_ASSERT(deviceCount != 0, "SelectPhysicaDevice : No available devices support Vulkan");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vulkan.GetInstance(), &deviceCount, devices.data());
    CP_INFO("SelectPhysicaDevice : Available devices:");
    for (auto&& device : devices)
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      CP_INFO_CONT("\t%s", deviceProperties.deviceName);
    }
    for (auto&& device : devices)
    {
      if (IsPhysicalDeviceSuitable(device))
      {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        physicalDevice = device;
        CP_INFO("SelectPhysicaDevice : Selecting device: %s", deviceProperties.deviceName);
        break;
      }
    }
    CP_ASSERT(physicalDevice != VK_NULL_HANDLE, "SelectPhysicaDevice : Failed to find suitable GPU");
  }

  void Device::InitializeLogicalDevice()
  {
    QueueFamiliesQuery query{vulkan.GetWindow().GetSurface(), physicalDevice};

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

    CP_VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "InitializeLogicalDevice : Failed to initialize logical device");

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
    CP_VK_ASSERT(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool), "InitializeCommandPool : Failed to initialize command pool");
  }

  bool Device::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return false;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    if (!deviceFeatures.fillModeNonSolid || !deviceFeatures.samplerAnisotropy)
      return false;

    QueueFamiliesQuery query{vulkan.GetWindow().GetSurface(), device};
    if (!query.AllRequiredFamiliesSupported())
      return false;

    if (!CheckDeviceExtensionSupport(device))
      return false;

    SwapChainSupportDetails details{vulkan.GetWindow().GetSurface(), device};
    if (!details.Valid())
      return false;

    return true;
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
