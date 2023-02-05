#pragma once

#include "Common.h"
namespace Copium
{

  class DebugMessenger
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DebugMessenger);
  public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    DebugMessenger(VkInstance instance)
      : instance{instance}
    {
  #ifndef NDEBUG
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
      CP_VK_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "DebugMessenger : Failed to initialze debug messenger");
  #endif
    }

    ~DebugMessenger()
    {
  #ifndef NDEBUG
      vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  #endif
    }

    static void AddRequiredExtensions(std::vector<const char*>* extensions)
    {
  #ifndef NDEBUG
      extensions->emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  #endif
    }

    static void AddRequiredLayers(std::vector<const char*>* layers)
    {
  #ifndef NDEBUG
      layers->emplace_back("VK_LAYER_KHRONOS_validation");
  #endif
    }

  private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData) 
    {
      if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
      {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
          CP_ABORT("DebugCallback : %s", pCallbackData->pMessage);
        else
          CP_WARN("DebugCallback : %s", pCallbackData->pMessage);
      }
      return VK_FALSE;
    }

    static VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator,
                                                   VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
      auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
      if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    static void vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                VkDebugUtilsMessengerEXT debugMessenger,
                                                const VkAllocationCallbacks* pAllocator) {
      auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
      if (func != nullptr) {
          func(instance, debugMessenger, pAllocator);
      }
    }
  };
}
