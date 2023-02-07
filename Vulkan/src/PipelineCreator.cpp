#include "PipelineCreator.h"

#include "Common.h"

namespace Copium
{
	PipelineCreator::PipelineCreator(VkRenderPass renderPass, const std::string& vertexShader, const std::string& fragmentShader)
		: vertexShader{vertexShader}, 
		  fragmentShader{fragmentShader}, 
		  renderPass{renderPass}
	{}

	void PipelineCreator::SetVertexDescriptor(const VertexDescriptor& descriptor)
	{
		vertexDescriptor = descriptor;
	}

	void PipelineCreator::AddDescriptorSetLayoutBinding(uint32_t set, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags)
	{
		CP_ASSERT(set <= descriptorSetLayouts.size(), "AddDescriptorSetLayoutBinding : Cannot add descriptor set with set number greater than the current set count");
		descriptorSetLayouts[set].emplace_back(DescriptorSetBinding{binding, type, count, stageFlags});
	}

	void PipelineCreator::SetPrimitiveTopology(VkPrimitiveTopology primitiveTopology)
	{
		topology = primitiveTopology;
	}

	void PipelineCreator::SetCullMode(VkCullModeFlags flags)
	{
		cullMode = flags;
	}

	void PipelineCreator::SetCullFrontFace(VkFrontFace cullFrontFace)
	{
		frontFace = cullFrontFace;
	}
}