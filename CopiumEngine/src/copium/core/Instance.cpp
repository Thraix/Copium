#include "Instance.h"

#include <GLFW/glfw3.h>

#include "copium/util/Common.h"

namespace Copium
{
  Instance::Instance(const std::string& applicationName)
  {
    InitializeInstance(applicationName);
    InitializeDebugMessenger();
  }

  Instance::~Instance()
  {
    debugMessenger.reset();
    vkDestroyInstance(instance, nullptr);
  }

  Instance::operator VkInstance() const
  {
    return instance;
  }

  void Instance::InitializeInstance(const std::string& applicationName)
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

    CP_INFO("Supported Extensions:");
    for (auto&& extension : extensions)
    {
      CP_INFO_CONT("\t%s", extension.extensionName);
    }

    std::vector<const char*> layers{};
    DebugMessenger::AddRequiredLayers(&layers);
    CP_ASSERT(CheckLayerSupport(layers), "Some required layers are not supported");

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    CP_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create instance");
  }

  void Instance::InitializeDebugMessenger()
  {
    debugMessenger = std::make_unique<DebugMessenger>(*this);
  }

  std::vector<const char*> Instance::GetRequiredExtensions()
  {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions{glfwExtensions, glfwExtensions + glfwExtensionCount};

    debugMessenger->AddRequiredExtensions(&extensions);

    return extensions;
  }

  bool Instance::CheckLayerSupport(const std::vector<const char*>& layers)
  {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    CP_INFO("Supported Layers:");
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
}
