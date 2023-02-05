#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <set>

#include "DebugMessenger.h"
#include "QueueFamilies.h"
#include "SwapChain.h"
#include "Timer.h"

namespace Copium
{
  class Instance final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Instance);
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
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    std::unique_ptr<SwapChain> swapChain;
    int flightIndex;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkCommandPool commandPool;
    bool framebufferResized = false;

    int frameCount = 0;
    Timer timer;

  public:
    Instance(const std::string& applicationName)
    {
      timer.Start();
      InitializeWindow(applicationName);
      InitializeInstance(applicationName);
      InitializeDebugMessenger();
      InitializeSurface();
      SelectPhysicalDevice();
      InitializeLogicalDevice();
      InitializeCommandPool();
      InitializeSwapChain();
      InitializeSyncObjects();
      CP_INFO("Instance : Initialized Vulkan in %f seconds", timer.Elapsed());
    }

    ~Instance()
    {
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
      {
        vkDestroyFence(device, inFlightFences[i], nullptr);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
      }
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
      vkWaitForFences(device, 1, &inFlightFences[flightIndex], VK_TRUE, UINT64_MAX);

      if (!swapChain->BeginPresent(imageAvailableSemaphores[flightIndex]))
        return false;

      vkResetFences(device, 1, &inFlightFences[flightIndex]);
      return true;
    }

    bool EndPresent()
    {
      swapChain->EndPresent(presentQueue, &renderFinishedSemaphores[flightIndex], framebufferResized);

      framebufferResized = false;
      flightIndex = (flightIndex + 1) % MAX_FRAMES_IN_FLIGHT;
      return !glfwWindowShouldClose(window);
    }

    void SubmitGraphicsQueue(const std::vector<VkCommandBuffer>& commandBuffers)
    {
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &imageAvailableSemaphores[flightIndex];
      submitInfo.pWaitDstStageMask = waitStages;
      submitInfo.commandBufferCount = commandBuffers.size();
      submitInfo.pCommandBuffers = commandBuffers.data();
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &renderFinishedSemaphores[flightIndex];

      CP_VK_ASSERT(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[flightIndex]), "SubmitGraphicsQueue : Failed to submit command buffer");
    }

    VkInstance GetInstance() const
    {
      return instance;
    }

    GLFWwindow* GetWindow() const
    {
      return window;
    }

    VkSurfaceKHR GetSurface() const
    {
      return surface;
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

    int GetFlightIndex() const
    {
      return flightIndex;
    }

    int GetMaxFramesInFlight() const
    {
      return MAX_FRAMES_IN_FLIGHT;
    }

    const SwapChain& GetSwapChain() const
    {
      return *swapChain;
    }

    // TODO: Create Device class and move this there
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
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

  private:
    void InitializeWindow(const std::string& applicationName)
    {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


#if defined(FULLSCREEN)
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      window = glfwCreateWindow(mode->width, mode->height, applicationName.c_str(), glfwGetPrimaryMonitor(), nullptr);
#elif defined(BORDERLESS_WINDOWED)
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      window = glfwCreateWindow(mode->width, mode->height, applicationName.c_str(), nullptr, nullptr);
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#else
      window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, applicationName.c_str(), nullptr, nullptr);
#endif
      CP_ASSERT(window, "InitializeWindow : Failed to initialize glfw window");

      glfwSetWindowUserPointer(window, this);
      glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    }

    void InitializeInstance(const std::string& applicationName)
    {
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = applicationName.c_str();
      appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.pEngineName = "Copium Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
      appInfo.apiVersion = VK_API_VERSION_1_1;

      std::vector<const char*> requiredExtensions = GetRequiredExtensions();

      uint32_t extensionCount;
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
      std::vector<VkExtensionProperties> extensions{extensionCount};
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

      CP_INFO("InitiaizeInstace : Supported Extensions:");
      for (auto&& extension : extensions)
      {
        CP_INFO_CONT("\t%s", extension.extensionName);
      }

      std::vector<const char*> layers{};
      DebugMessenger::AddRequiredLayers(&layers);
      CP_ASSERT(CheckLayerSupport(layers), "InitializeInstance : Some required layers are not supported");

      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;
      createInfo.enabledExtensionCount = requiredExtensions.size();
      createInfo.ppEnabledExtensionNames = requiredExtensions.data();
      createInfo.enabledLayerCount = layers.size();
      createInfo.ppEnabledLayerNames = layers.data();
      CP_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance), "InitializeInstance : Failed to create instance");
    }

    void InitializeDebugMessenger()
    {
      debugMessenger = std::make_unique<DebugMessenger>(instance);
    }

    void InitializeSurface()
    {
      CP_VK_ASSERT(glfwCreateWindowSurface(instance, window, nullptr, &surface), "InitializeSurface : Failed to create Vulkan surface");
    }

    void SelectPhysicalDevice()
    {
      uint32_t deviceCount;
      vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
      CP_ASSERT(deviceCount != 0, "SelectPhysicaDevice : No available devices support Vulkan");

      std::vector<VkPhysicalDevice> devices(deviceCount);
      vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
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

    void InitializeLogicalDevice()
    {
      QueueFamiliesQuery query{surface, physicalDevice};

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

    void InitializeSwapChain()
    {
      swapChain = std::make_unique<SwapChain>(*this);
    }

    void InitializeCommandPool()
    {
      VkCommandPoolCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      createInfo.queueFamilyIndex = graphicsQueueIndex;
      CP_VK_ASSERT(vkCreateCommandPool(device, &createInfo, nullptr, &commandPool), "InitializeCommandPool : Failed to initialize command pool");
    }

    void InitializeSyncObjects()
    {
      imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
      inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
      VkSemaphoreCreateInfo semaphoreCreateInfo{};
      semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
      for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
      {
        CP_VK_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "InitializeSyncObjects : Failed to initialize available image semaphore");
        CP_VK_ASSERT(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "InitializeSyncObjects : Failed to initialize render finished semaphore");

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        CP_VK_ASSERT(vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]), "InitializeSyncObjects : Failed to initialize in flight fence");
      }
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

      CP_INFO("CheckLayerSupport : Supported Layers:");
      for (auto&& availableLayer : availableLayers)
      {
        CP_INFO_CONT("\t%s", availableLayer.layerName);
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
      if (!deviceFeatures.fillModeNonSolid || !deviceFeatures.samplerAnisotropy)
        return false;

      QueueFamiliesQuery query{surface, device};
      if (!query.AllRequiredFamiliesSupported())
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
}
