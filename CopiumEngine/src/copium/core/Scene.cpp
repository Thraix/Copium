#include "copium/core/Scene.h"

#include "copium/asset/AssetManager.h"
#include "copium/asset/AssetMeta.h"
#include "copium/asset/AssetRef.h"
#include "copium/core/Vulkan.h"
#include "copium/ecs/Entity.h"
#include "copium/ecs/System.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/example/FrameCountSystem.h"
#include "copium/example/MouseFollowSystem.h"
#include "copium/example/RenderSystem.h"
#include "copium/example/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Copium
{
  Scene::Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool)
  {
    renderer = std::make_unique<Renderer>();
    descriptorSetRenderer = renderer->GetGraphicsPipeline().CreateDescriptorSet(descriptorPool, 1);
    ecs = std::make_unique<ECSManager>();
    ecs->AddSystem<RenderSystem>(renderer.get(), descriptorSetRenderer.get(), &commandBuffer); // better way to store the RenderSystem data?
    ecs->AddSystem<FrameCountSystem>().Before<FrameCountSystem>();
    ecs->AddSystem<MouseFollowSystem>();

    // TODO: Load from scene file
    for (int y = 0; y < 10; y++)
    {
      for (int x = 0; x < 10; x++)
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-1 + x * 0.2 + 0.05, -1 + y * 0.2 + 0.05}, glm::vec2{0.1, 0.1});
        entity.AddComponent<ColorC>(glm::vec3{x * 0.1f, y * 0.1f, 1.0f});
      }
    }

    float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;
    Entity entityFox = Entity::Create(ecs.get());
    entityFox.AddComponent<TransformC>(glm::vec2{-0.9f, -0.4f}, glm::vec2{0.8f, 0.8f});
    entityFox.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});

    Entity entityFontAtlas = Entity::Create(ecs.get());
    entityFontAtlas.AddComponent<TransformC>(glm::vec2{0.1f, -0.4f}, glm::vec2{0.8, 0.8});
    entityFontAtlas.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("font.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});

    Entity entityMouse = Entity::Create(ecs.get());
    entityMouse.AddComponent<TransformC>(glm::vec2(0.1), glm::vec2{0.2});
    entityMouse.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox2.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityMouse.AddComponent<MouseFollowC>();

    Entity entityText = Entity::Create(ecs.get());
    entityText.AddComponent<TransformC>(glm::vec2{-aspect + 0.01, 0.94}, glm::vec2{1.0});
    entityText.AddComponent<TextC>(AssetRef{AssetManager::LoadAsset("font.meta")}, std::to_string(0) + " fps", 0.06f);
    entityText.AddComponent<FrameCountC>();
  }

  void Scene::Update()
  {
    ecs->UpdateSystems();
  }

  EventResult Scene::OnEvent(const Event& event)
  {
    ecs->UpdateSystems(EventSignal{event});
    return EventResult::Continue;
  }
}