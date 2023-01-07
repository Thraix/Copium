#pragma once

#include <vulkan/vulkan.hpp>
#include <iostream>

#define ASSERT(Function, message) if(!(Function)) { throw std::runtime_error(message); } while(false)
#define VK_ASSERT(Function, message) if(Function != VK_SUCCESS) { throw std::runtime_error(message); } while(false)

VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, 
	                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
	                                      const VkAllocationCallbacks* pAllocator, 
	                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, 
	                                 VkDebugUtilsMessengerEXT debugMessenger, 
	                                 const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
