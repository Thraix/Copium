#pragma once

#include "Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{

  class DebugMessenger final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DebugMessenger);
  public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    DebugMessenger(VkInstance instance);
    ~DebugMessenger();

    static void AddRequiredExtensions(std::vector<const char*>* extensions);
    static void AddRequiredLayers(std::vector<const char*>* layers);
 

  private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);


    static VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                                   const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator,
                                                   VkDebugUtilsMessengerEXT* pDebugMessenger);


    static void vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                VkDebugUtilsMessengerEXT debugMessenger,
                                                const VkAllocationCallbacks* pAllocator);
  };
}
