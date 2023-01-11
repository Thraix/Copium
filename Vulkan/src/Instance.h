#pragma once

#include <vulkan/vulkan.hpp>
#include <set>

#include "DebugMessenger.h"
#include "QueueFamilies.h"
#include "SwapChain.h"

class Instance final
{
private:
	static const int MAX_FRAMES_IN_FLIGHT = 2;
  static const int WINDOW_WIDTH = 1920;
  static const int WINDOW_HEIGHT = 1080;


  VkInstance instance;
  GLFWwindow* window;
  VkSurfaceKHR surface;
  std::unique_ptr<DebugMessenger> debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  std::unique_ptr<SwapChain> swapChain;
  VkCommandPool commandPool;
  bool framebufferResized = false;

public:
  Instance(const std::string& applicationName)
  {
    InitializeWindow(applicationName);
    InitializeInstance(applicationName);
    InitializeDebugMessenger();
		InitializeSurface();
    SelectPhysicalDevice();
    InitializeLogicalDevice();
    InitializeSwapChain();
    InitializeCommandPool();
  }

  ~Instance()
  {
		vkDestroyCommandPool(device, commandPool, nullptr);
    swapChain.reset();
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    debugMessenger.reset();
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
  }

  bool BeginPresent()
  {
    if (!swapChain->BeginPresent())
      return true;
  }

  bool EndPresent()
  {
    swapChain->EndPresent(presentQueue, framebufferResized);
    return !glfwWindowShouldClose(window);
  }

  void SubmitGraphicsQueue(const std::vector<VkCommandBuffer>& commandBuffers)
  {
    VkSemaphore waitSemaphores[] = {swapChain->GetAvailableImageSemaphore()};
    VkSemaphore signalSemaphores[] = {swapChain->GetRenderFinishedSemaphore()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_ASSERT(vkQueueSubmit(graphicsQueue, 1, &submitInfo, swapChain->GetInFlightFence()), "Failed to submit command buffer");
  }

  VkInstance GetInstance() const
  {
    return instance;
  }

  VkPhysicalDevice GetPhysicalDevice() const
  {
    return physicalDevice;
  }

  VkDevice GetDevice() const
  {
    return device;
  }

  VkCommandPool GetCommandPool() const
  {
    return commandPool;
  }

  VkQueue GetGraphicsQueue() const
  {
    return graphicsQueue;
  }

  int GetMaxFramesInFlight()
  {
    return MAX_FRAMES_IN_FLIGHT;
  }

  const SwapChain& GetSwapChain() const
  {
    return *swapChain;
  }

private:
  void InitializeWindow(const std::string& applicationName)
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, applicationName.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
  }

  void InitializeInstance(const std::string& applicationName)
  {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Greet Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> requiredExtensions = GetRequiredExtensions();

    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions{extensionCount};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "Supported Extensions: " << std::endl;
    for (auto&& extension : extensions)
    {
      std::cout << "\t" << extension.extensionName << std::endl;
    }

    std::vector<const char*> layers{};
    DebugMessenger::AddRequiredLayers(&layers);
    ASSERT(CheckLayerSupport(layers), "Some required layers are not supported");

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create instance");
  }

  void InitializeDebugMessenger()
  {
    debugMessenger = std::make_unique<DebugMessenger>(instance);
  }

  void InitializeSurface()
  {
    VK_ASSERT(glfwCreateWindowSurface(instance, window, nullptr, &surface), "Failed to create Vulkan surface");
  }

  void SelectPhysicalDevice()
  {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    ASSERT(deviceCount != 0, "No available devices support Vulkan");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::cout << "Available devices: " << std::endl;
    for (auto&& device : devices)
    {
      VkPhysicalDeviceProperties deviceProperties;
      vkGetPhysicalDeviceProperties(device, &deviceProperties);
      std::cout << "\t" << deviceProperties.deviceName << std::endl;
    }
    for (auto&& device : devices)
    {
      if (IsPhysicalDeviceSuitable(device))
      {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        physicalDevice = device;
        std::cout << "Selecting device: " << deviceProperties.deviceName << std::endl;
        break;
      }
    }
    ASSERT(physicalDevice != VK_NULL_HANDLE, "Failed to find suitable GPU");
  }

  void InitializeLogicalDevice()
  {
    QueueFamilies queueFamilies{surface, physicalDevice};

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies{queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value()};
    for(auto&& queueFamily : uniqueQueueFamilies)
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
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledExtensionCount = deviceExtensions.size();

    VK_ASSERT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to initialize logical device");

    vkGetDeviceQueue(device, queueFamilies.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFamilies.presentFamily.value(), 0, &presentQueue);
  }

  void InitializeSwapChain()
  {
    swapChain = std::make_unique<SwapChain>(window, surface, device, physicalDevice);
  }

  void InitializeCommandPool()
  {
    QueueFamilies queueFamilies{surface, physicalDevice};

    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
    VK_ASSERT(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool), "Failed to initialize command pool");
	}

  std::vector<const char*> GetRequiredExtensions()
  {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions{glfwExtensions, glfwExtensions + glfwExtensionCount};

    debugMessenger->AddRequiredExtensions(&extensions);

    return extensions;
  }

  bool CheckLayerSupport(const std::vector<const char*>& layers)
  {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::cout << "Supported Layers: " << std::endl;
    for (auto&& availableLayer : availableLayers)
    {
      std::cout << "\t" << availableLayer.layerName << std::endl;
    }

    for (auto&& layer : layers)
    {
      bool layerFound = false;
      for (auto&& availableLayer : availableLayers)
      {
        if (std::strcmp(layer, availableLayer.layerName) == 0)
        {
          layerFound = true;
          break;
        }
      }
      if (!layerFound)
        return false;
    }
    return true;
  }

  bool IsPhysicalDeviceSuitable(VkPhysicalDevice device)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
      return false;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    if (!deviceFeatures.fillModeNonSolid)
      return false;

    QueueFamilies queueFamilies{surface, device};
    if (!queueFamilies.AllRequiredFamiliesSupported())
      return false;

    if (!CheckDeviceExtensionSupport(device))
      return false;
		SwapChainSupportDetails details{surface, device};
    if (!details.Valid())
      return false;

    return true;
  }

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
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

  std::vector<const char*> GetRequiredDeviceExtensions()
  {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  }

  static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
  {
    Instance* instance = static_cast<Instance*>(glfwGetWindowUserPointer(window));
    instance->framebufferResized = true;
  }

};
