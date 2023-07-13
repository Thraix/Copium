#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/renderer/Renderer.h"
#include "copium/asset/AssetManager.h"
#include "copium/core/Vulkan.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  class RenderSystem : public System<RenderableC, TransformC>
  {
  private:
    // Find better way to store these?
    Renderer* renderer;
    DescriptorSet* descriptorSet;
    CommandBuffer* commandBuffer;
    glm::mat4* viewMatrix;
    glm::mat4* projectionMatrix;

    bool renderColliders = false;
  public:
    RenderSystem(Renderer* renderer, DescriptorSet* descriptorSet, CommandBuffer* commandBuffer, glm::mat4* viewMatrix, glm::mat4* projectionMatrix)
      : renderer{renderer},
        descriptorSet{descriptorSet},
        commandBuffer{commandBuffer},
        viewMatrix{viewMatrix},
        projectionMatrix{projectionMatrix}
    {}

    void RunEntity(Entity entity, RenderableC& renderable, TransformC& transform) override
    {
      if (entity.HasComponent<TextC>())
      {
        const TextC& text = entity.GetComponent<TextC>();
        renderer->Text(text.text, transform.position, AssetManager::GetAsset<Font>(text.font), text.fontSize);
      }
      else if (entity.HasComponent<ColorC>())
      {
        const ColorC& color = entity.GetComponent<ColorC>();
        renderer->Quad(transform.position, transform.size, color.color);
      }
      else if (entity.HasComponent<TextureC>())
      {
        const TextureC& texture = entity.GetComponent<TextureC>();
        renderer->Quad(transform.position, transform.size, AssetManager::GetAsset<Sampler>(texture.asset), texture.texCoord1, texture.texCoord2);
      }
    }

    void RenderCollider(Entity entity, DynamicColliderC& dynamicCollider, TransformC& transform)
    {
      renderer->Quad(transform.position + transform.size * dynamicCollider.colliderOffset, transform.size * dynamicCollider.colliderSize, glm::vec3{0.8, 0.1, 0.1});
    }

    void Run() override
    {
      if (Input::IsKeyPressed(CP_KEY_K))
        renderColliders = !renderColliders;
      UniformBuffer& uniformBuffer = descriptorSet->GetUniformBuffer("ubo");
      uniformBuffer.Set("projection", *projectionMatrix);
      uniformBuffer.Set("view", *viewMatrix);
      uniformBuffer.Update();

      renderer->SetDescriptorSet(*descriptorSet);
      renderer->Begin(*commandBuffer);
      System::Run();
      if (renderColliders)
      {
        manager->Each<DynamicColliderC, TransformC>([&](EntityId entityId, DynamicColliderC& dynamicCollider, TransformC& transform) { RenderCollider(Entity{manager, entityId}, dynamicCollider, transform); });
      }

      renderer->End();
    }
  };

}
