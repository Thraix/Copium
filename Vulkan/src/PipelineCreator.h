#pragma once

#include "Common.h"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.hpp>

class PipelineCreator
{
	struct DescriptorSetLayout
	{
		VkDescriptorType type;
		VkShaderStageFlags flags;
	};
	friend class Pipeline;
private:
	std::map<uint32_t, DescriptorSetLayout> descriptorSetLayouts{};

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

	void AddDescriptorSetLayoutBinding(uint32_t set, VkDescriptorType type, VkShaderStageFlags stageFlags)
	{
		descriptorSetLayouts.emplace(set, DescriptorSetLayout{type, stageFlags});
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
