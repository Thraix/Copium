#include "copium/core/DebugMessenger.h"

#include "copium/core/Instance.h"

namespace Copium
{
#ifndef NDEBUG
  DebugMessenger::DebugMessenger(Instance& instance)
    : instance{instance}
  {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr;
    CP_VK_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "Failed to initialze debug messenger");
  }

  DebugMessenger::~DebugMessenger()
  {
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  void DebugMessenger::AddRequiredExtensions(std::vector<const char*>* extensions)
  {
    extensions->emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  void DebugMessenger::AddRequiredLayers(std::vector<const char*>* layers)
  {
    layers->emplace_back("VK_LAYER_KHRONOS_validation");
  }
#else
  DebugMessenger::DebugMessenger(Instance& instance)
    : instance{instance}
  {}

  DebugMessenger::~DebugMessenger()
  {}

  void DebugMessenger::AddRequiredExtensions(std::vector<const char*>* extensions)
  {}

  void DebugMessenger::AddRequiredLayers(std::vector<const char*>* layers)
  {}
#endif

  VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                               VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                               void* pUserData) 
  {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
      if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        CP_ABORT("%s", pCallbackData->pMessage);
      else
        CP_ABORT("%s", pCallbackData->pMessage);
    }
    return VK_FALSE;
  }

  VkResult DebugMessenger::vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                          const VkAllocationCallbacks* pAllocator,
                                                          VkDebugUtilsMessengerEXT* pDebugMessenger)
  {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  void DebugMessenger::vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                       VkDebugUtilsMessengerEXT debugMessenger,
                                                       const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
  }

}
