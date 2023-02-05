#pragma once

#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "Framebuffer.h"
#include "IndexBuffer.h"
#include "Instance.h"
#include "Pipeline.h"
#include "Texture2D.h"
#include "Timer.h"
#include "UniformBuffer.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexPassthrough.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

namespace Copium
{
  const std::vector<Vertex> vertices = {
    Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{ 0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    Vertex{{ 0.5f, 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{-0.5f, 0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    Vertex{{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    Vertex{{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    Vertex{{ 0.5f, 0.0f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    Vertex{{-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
  };

  const std::vector<uint16_t> indices = {
      0, 1, 2, 2, 3, 0,
      4, 5, 6, 6, 7, 4
  };

  const std::vector<VertexPassthrough> verticesPassthrough = {
    VertexPassthrough{{-1.0f, -1.0f}},
    VertexPassthrough{{ 1.0f, -1.0f}},
    VertexPassthrough{{ 1.0f,  1.0f}},
    VertexPassthrough{{-1.0f,  1.0f}},
  };

  const std::vector<uint16_t> indicesPassthrough = {
      0, 1, 2, 2, 3, 0,
  };

  struct alignas(64) ShaderUniform
  {
    alignas(16) glm::mat4 projection;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 model;
    alignas(16) glm::vec3 lightPos;
  };

  class Application final
  {
  private:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Pipeline> graphicsPipeline;
    std::unique_ptr<Texture2D> texture2D;
    std::unique_ptr<UniformBuffer> shaderUniformBuffer;
    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<DescriptorSet> descriptorSet;
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<CommandBuffer> commandBuffer;

    std::unique_ptr<Framebuffer> framebuffer;
    std::unique_ptr<Pipeline> graphicsPipelinePassthrough;
    std::unique_ptr<VertexBuffer> vertexBufferPassthrough;
    std::unique_ptr<IndexBuffer> indexBufferPassthrough;
    std::unique_ptr<DescriptorSet> descriptorSetPassthrough;

  public:
    Application()
    {
      InitializeInstance();
      InitializeFrameBuffer();
      InitializeGraphicsPipeline();
      InitializeTextureSampler();
      InitializeUniformBuffer();
      InitializeDescriptorSets();
      InitializeVertexBuffer();
      InitializeIndexBuffer();
      InitializeCommandBuffer();
    }

    ~Application()
    {
      vkDeviceWaitIdle(instance->GetDevice());
    }

    Application(Application&&) = delete;
    Application(const Application&) = delete;
    Application& operator=(Application&&) = delete;
    Application& operator=(const Application&) = delete;

    bool Update()
    {
      if (framebuffer->GetWidth() != instance->GetSwapChain().GetExtent().width || framebuffer->GetHeight() != instance->GetSwapChain().GetExtent().height)
      {
        framebuffer->Resize(instance->GetSwapChain().GetExtent().width / 8, instance->GetSwapChain().GetExtent().height / 8);
        descriptorSetPassthrough->AddSampler(framebuffer->GetColorAttachment(), 0);
      }
      if (!instance->BeginPresent())
      {
        return true;
      }

      RecordCommandBuffer();
      commandBuffer->SubmitAsGraphicsQueue();

      return instance->EndPresent();
    }

  private:

    void InitializeInstance()
    {
      instance = std::make_unique<Instance>("Copium Engine");
    }

    void InitializeFrameBuffer()
    {
      framebuffer = std::make_unique<Framebuffer>(*instance, instance->GetSwapChain().GetExtent().width, instance->GetSwapChain().GetExtent().height);
    }

    void InitializeTextureSampler()
    {
      texture2D = std::make_unique<Texture2D>(*instance, "res/textures/texture.png");
    }

    void InitializeUniformBuffer()
    {
      shaderUniformBuffer = std::make_unique<UniformBuffer>(*instance, sizeof(ShaderUniform));
    }

    void InitializeDescriptorSets()
    {
      descriptorPool = std::make_unique<DescriptorPool>(*instance);

      descriptorSet = std::make_unique<DescriptorSet>(*instance, *descriptorPool, graphicsPipeline->GetDescriptorSetLayout(0));
      descriptorSet->AddUniform(*shaderUniformBuffer, 0);
      descriptorSet->AddSampler(*texture2D, 1);

      descriptorSetPassthrough = std::make_unique<DescriptorSet>(*instance, *descriptorPool, graphicsPipelinePassthrough->GetDescriptorSetLayout(0));
      descriptorSetPassthrough->AddSampler(framebuffer->GetColorAttachment(), 0);
    }

    void InitializeGraphicsPipeline()
    {
      PipelineCreator creator{framebuffer->GetRenderPass(), "res/shaders/shader.vert", "res/shaders/shader.frag"};
      creator.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
      creator.AddDescriptorSetLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
      creator.SetVertexDescriptor(Vertex::GetDescriptor());
      creator.SetCullMode(VK_CULL_MODE_NONE);
      graphicsPipeline = std::make_unique<Pipeline>(*instance, creator);

      PipelineCreator creatorPassthrough{instance->GetSwapChain().GetRenderPass(), "res/shaders/passthrough.vert", "res/shaders/passthrough.frag"};
      creatorPassthrough.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
      creatorPassthrough.SetVertexDescriptor(VertexPassthrough::GetDescriptor());
      creatorPassthrough.SetCullMode(VK_CULL_MODE_NONE);
      graphicsPipelinePassthrough = std::make_unique<Pipeline>(*instance, creatorPassthrough);
    }

    void InitializeVertexBuffer()
    {
      vertexBuffer = std::make_unique<VertexBuffer>(*instance, Vertex::GetDescriptor(), vertices.size());
      vertexBuffer->Update(0, (void*)vertices.data());

      vertexBufferPassthrough = std::make_unique<VertexBuffer>(*instance, VertexPassthrough::GetDescriptor(), verticesPassthrough.size());
      vertexBufferPassthrough->Update(0, (void*)verticesPassthrough.data());
    }

    void InitializeIndexBuffer()
    {
      indexBuffer = std::make_unique<IndexBuffer>(*instance, indices.size());
      indexBuffer->UpdateStaging((void*)indices.data());

      indexBufferPassthrough = std::make_unique<IndexBuffer>(*instance, indicesPassthrough.size());
      indexBufferPassthrough->UpdateStaging((void*)indicesPassthrough.data());
    }

    void InitializeCommandBuffer()
    {
      commandBuffer = std::make_unique<CommandBuffer>(*instance, CommandBuffer::Type::Dynamic);
    }

    void RecordCommandBuffer()
    {
      commandBuffer->Begin();

      framebuffer->Bind(*commandBuffer);
      graphicsPipeline->Bind(*commandBuffer);

      UpdateUniformBuffer();

      vertexBuffer->Bind(*commandBuffer);
      indexBuffer->Bind(*commandBuffer);

      graphicsPipeline->SetDescriptorSet(0, *descriptorSet);
      graphicsPipeline->BindDescriptorSets(commandBuffer->GetHandle());

      indexBuffer->Draw(*commandBuffer);
      framebuffer->Unbind(*commandBuffer);

      instance->GetSwapChain().BeginFrameBuffer(*commandBuffer);

      graphicsPipelinePassthrough->Bind(*commandBuffer);
      graphicsPipelinePassthrough->SetDescriptorSet(0, *descriptorSetPassthrough);
      graphicsPipelinePassthrough->BindDescriptorSets(commandBuffer->GetHandle());
      vertexBufferPassthrough->Bind(*commandBuffer);
      indexBufferPassthrough->Bind(*commandBuffer);
      indexBufferPassthrough->Draw(*commandBuffer);

      instance->GetSwapChain().EndFrameBuffer(*commandBuffer);
      commandBuffer->End();
    }

    void UpdateUniformBuffer()
    {
      static Timer startTimer;

      float time = startTimer.Elapsed();
      ShaderUniform shaderUniform;
      shaderUniform.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      shaderUniform.projection = glm::perspective(glm::radians(45.0f), framebuffer->GetWidth() / (float)framebuffer->GetHeight(), 0.1f, 10.0f);
      shaderUniform.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      shaderUniform.projection[1][1] *= -1;
      shaderUniform.lightPos = glm::rotate(glm::mat4{1.0f}, time * glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::vec4{0.3, 0.1, 0, 1};

      shaderUniformBuffer->Update(shaderUniform);
    }
  };
}
