#pragma once

#include "VulkanException.h"
#include <vulkan/vulkan.hpp>
#include <iostream>

#define CP_DEBUG(format, ...) std::cout << "[DBG] " << StringFormat(format, __VA_ARGS__) << std::endl
#define CP_INFO(format, ...)  std::cout << "[INF] " << StringFormat(format, __VA_ARGS__) << std::endl
#define CP_WARN(format, ...)  std::cout << "[WRN] " << StringFormat(format, __VA_ARGS__) << std::endl
#define CP_ERR(format, ...)   std::cout << "[ERR] " << StringFormat(format, __VA_ARGS__) << std::endl

#define CP_UNIMPLEMENTED() CP_WARN("%s is unimplemented", __FUNCTION__)
#define CP_ASSERT(Function, format, ...) if(!(Function)) { throw std::runtime_error(StringFormat(format, __VA_ARGS__)); } while(false)
#define CP_VK_ASSERT(Function, format, ...) if(Function != VK_SUCCESS) { throw VulkanException(StringFormat(format, __VA_ARGS__)); } while(false)
#define CP_DELETE_COPY_AND_MOVE_CTOR(ClassName) \
	ClassName(ClassName&&) = delete; \
	ClassName(const ClassName&) = delete; \
	ClassName& operator=(ClassName&&) = delete; \
	ClassName& operator=(const ClassName&) = delete

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

template<typename ... Args>
std::string StringFormat(const std::string& format, Args... args)
{
    int size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
		CP_ASSERT(size > 0, "Error during formatting");
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

