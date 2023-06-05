#include "copium/core/Scene.h"

#include "copium/asset/AssetManager.h"
#include "copium/asset/AssetMeta.h"
#include "copium/asset/AssetRef.h"
#include "copium/core/Vulkan.h"
#include "copium/ecs/Entity.h"
#include "copium/ecs/System.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/example/CameraFollowPlayerSystem.h"
#include "copium/example/CameraUpdateSystem.h"
#include "copium/example/Components.h"
#include "copium/example/FrameCountSystem.h"
#include "copium/example/HealthChangeSystem.h"
#include "copium/example/HealthComponentListener.h"
#include "copium/example/HealthDisplaySystem.h"
#include "copium/example/MouseFollowSystem.h"
#include "copium/example/ColliderSystem.h"
#include "copium/example/PhysicsSystem.h"
#include "copium/example/PlayerControllerSystem.h"
#include "copium/example/RenderSystem.h"
#include "copium/example/PickupSystem.h"

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

    ecs->AddSystem<PlayerControllerSystem>();
    ecs->AddSystem<PhysicsSystem>();
    ecs->AddSystem<ColliderSystem>();
    ecs->AddSystem<PickupSystem>();
    ecs->AddSystem<HealthChangeSystem>();
    ecs->AddSystem<HealthDisplaySystem>();
    ecs->AddSystem<CameraFollowPlayerSystem>();
    ecs->AddSystem<CameraUpdateSystem>(&viewMatrix, &projectionMatrix, &invPvMatrix);
    ecs->AddSystem<MouseFollowSystem>(&invPvMatrix);
    ecs->AddSystem<FrameCountSystem>();
    ecs->AddSystem<RenderSystem>(renderer.get(), descriptorSetRenderer.get(), &commandBuffer, &viewMatrix, &projectionMatrix); // better way to store the RenderSystem data?
    ecs->SetComponentListener<HealthComponentListener>();

    // TODO: Load from scene file
    for (int y = 0; y < 10; y++)
    {
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-10.0f + 0.4f, -11.0f + y * 1.6 + 0.4f}, glm::vec2{0.8f, 0.8f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
      }
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{10.0f - 0.4f, -10.0f + y * 1.6 + 0.4f}, glm::vec2{0.8f, 0.8f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
      }
    }
    for (int x = 0; x < 10; x++)
    {
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-11.0f + x * 1.6 + 0.4f, -10.0f + 0.4f, }, glm::vec2{0.8f, 0.8f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
      }
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-11.0f + x * 1.6 + 0.4f, 10.0f - 0.4f, }, glm::vec2{0.8f, 0.8f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
      }
    }
    for (int y = 0; y < 10; y++)
    {
      for (int x = 0; x < 10; x++)
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.6f + 0.4f, -10.0f + y * 1.6 + 0.4f}, glm::vec2{0.8f, 0.8f});
        entity.AddComponent<ColorC>(glm::vec3{x * 0.1f, y * 0.1f, 1.0f});
        entity.AddComponent<StaticColliderC>(false);
        entity.AddComponent<PickupC>();
      }
    }

    float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;
    Entity entityFox = Entity::Create(ecs.get());
    entityFox.AddComponent<TransformC>(glm::vec2{-0.9f, -0.4f}, glm::vec2{0.8f, 0.8f});
    entityFox.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityFox.AddComponent<StaticColliderC>(true);

    Entity entityFontAtlas = Entity::Create(ecs.get());
    entityFontAtlas.AddComponent<TransformC>(glm::vec2{0.1f, -0.4f}, glm::vec2{0.8, 0.8});
    entityFontAtlas.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("font.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityFontAtlas.AddComponent<StaticColliderC>(false);
    entityFontAtlas.AddComponent<PickupC>();

    Entity entityMouse = Entity::Create(ecs.get());
    entityMouse.AddComponent<TransformC>(glm::vec2(0.1), glm::vec2{0.2});
    entityMouse.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox2.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityMouse.AddComponent<MouseFollowC>();

    Entity entityText = Entity::Create(ecs.get());
    entityText.AddComponent<TransformC>(glm::vec2{-aspect * 10.0f + 0.1f, 9.4f}, glm::vec2{1.0});
    entityText.AddComponent<TextC>(AssetRef{AssetManager::LoadAsset("font.meta")}, std::to_string(0) + " fps", 0.6f);
    entityText.AddComponent<FrameCountC>();

    Entity entityCamera = Entity::Create(ecs.get());
    entityCamera.AddComponent<CameraC>(BoundingBox(-aspect, -1.0f, aspect, 1.0f), false);
    entityCamera.AddComponent<TransformC>(glm::vec2{0.0f}, glm::vec2{4.0f});

    Entity entityPlayer = Entity::Create(ecs.get());
    entityPlayer.AddComponent<PlayerC>(entityCamera);
    entityPlayer.AddComponent<HealthC>(10, 10);
    entityPlayer.AddComponent<PhysicsC>(0.1f, glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
    entityPlayer.AddComponent<TransformC>(glm::vec2{0.0f, 2.0f}, glm::vec2{1.0f});
    entityPlayer.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox2.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityPlayer.AddComponent<DynamicColliderC>(false, glm::vec2{0.0f});
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