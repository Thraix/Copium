#pragma once

#include <stdexcept>

class VulkanException : public std::runtime_error
{
public:
	VulkanException(const char* str)
		: runtime_error{str}
	{}
};
