#pragma once

#include "Common.h"

class DebugMessenger
{
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
		VK_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "Failed to initialze debug messenger");
#endif
	}

	~DebugMessenger()
	{
#ifndef NDEBUG
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
	}

	DebugMessenger(DebugMessenger&&) = delete;
	DebugMessenger(const DebugMessenger&) = delete;
	DebugMessenger& operator=(DebugMessenger&&) = delete;
	DebugMessenger& operator=(const DebugMessenger&) = delete;

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
			std::cerr << pCallbackData->pMessage << std::endl;
			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
				throw VulkanException(pCallbackData->pMessage);
		}
		return VK_FALSE;
	}

};
