#pragma once

#include "Common.h"
#include "VertexDescriptor.h"

#include <vulkan/vulkan.hpp>
#include <map>

class PipelineCreator
{
	struct DescriptorSetBinding
	{
		uint32_t binding;
		VkDescriptorType type;
		uint32_t count;
		VkShaderStageFlags flags;
	};
	friend class Pipeline;
private:
	std::map<uint32_t, std::vector<DescriptorSetBinding>> descriptorSetLayouts{};

	std::string vertexShader;
	std::string fragmentShader;
	VertexDescriptor vertexDescriptor{};
	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
	VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;

public:
	PipelineCreator(const std::string& vertexShader, const std::string& fragmentShader)
		: vertexShader{vertexShader}, fragmentShader{fragmentShader}
	{}

	void SetVertexDescriptor(const VertexDescriptor& descriptor)
	{
		vertexDescriptor = descriptor;
	}

	void AddDescriptorSetLayoutBinding(uint32_t set, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags)
	{
		CP_ASSERT(set <= descriptorSetLayouts.size(), "Cannot add descriptor set with set number greater than the current set count");
		descriptorSetLayouts[set].emplace_back(DescriptorSetBinding{binding, type, count, stageFlags});
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
