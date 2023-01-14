#pragma once

#include <stdexcept>

class VulkanException : public std::runtime_error
{
public:
	VulkanException(const std::string& str)
		: runtime_error{str.c_str()}
	{}
};
