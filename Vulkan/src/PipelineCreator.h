#pragma once

#include "Common.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.hpp>

class PipelineCreator
{
	friend class Pipeline;
private:
	std::set<uint32_t> vertexDescriptorSetLayouts{};
	std::set<uint32_t> fragmentDescriptorSetLayouts{};

	std::string vertexShader;
	std::string fragmentShader;
	VkVertexInputBindingDescription vertexInputBindingDescription;
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions{};
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
	VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;


public:
	PipelineCreator(const std::string& vertexShader, const std::string& fragmentShader)
		: vertexShader{vertexShader}, fragmentShader{fragmentShader}
	{}

	void SetVertexInputBindingDescription(VkVertexInputBindingDescription description)
	{
		vertexInputBindingDescription = description;
	}

	void SetVertexInputAttributeDescription(const std::vector<VkVertexInputAttributeDescription>& descriptions)
	{
		vertexInputAttributeDescriptions = descriptions;
	}

	void AddVertexDescriptorSetLayoutBinding(uint32_t binding)
	{
		vertexDescriptorSetLayouts.emplace(binding);
	}

	void AddFragmentDescriptorSetLayoutBinding(uint32_t binding)
	{
		fragmentDescriptorSetLayouts.emplace(binding);
	}

	void SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology)
	{
		topology = primitiveTopology;
	}

	void SetCullMode(VkCullModeFlags flags)
	{
		cullMode = flags;
	}

	void SetCullFrontFace(VkFrontFace cullFrontFace)
	{
		frontFace = cullFrontFace;
	}
};
