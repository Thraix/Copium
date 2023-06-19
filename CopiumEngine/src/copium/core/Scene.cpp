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
#include "copium/example/ColliderSystem.h"
#include "copium/example/Components.h"
#include "copium/example/DebugSystem.h"
#include "copium/example/AnimationSystem.h"
#include "copium/example/FrameCountSystem.h"
#include "copium/example/HealthChangeSystem.h"
#include "copium/example/HealthComponentListener.h"
#include "copium/example/HealthDisplaySystem.h"
#include "copium/example/MouseFollowSystem.h"
#include "copium/example/PhysicsSystem.h"
#include "copium/example/PickupSystem.h"
#include "copium/example/PlayerControllerSystem.h"
#include "copium/example/RenderSystem.h"
#include "copium/example/UiRenderSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Copium
{
  Scene::Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool)
  {
    renderer = std::make_unique<Renderer>();
    uiRenderer = std::make_unique<Renderer>();
    descriptorSetRenderer = renderer->GetGraphicsPipeline().CreateDescriptorSet(descriptorPool, 1);
    uiDescriptorSetRenderer = renderer->GetGraphicsPipeline().CreateDescriptorSet(descriptorPool, 1);
    ecs = std::make_unique<ECSManager>();

    ecs->AddSystem<PlayerControllerSystem>();
    ecs->AddSystem<PhysicsSystem>();
    ecs->AddSystem<ColliderSystem>();
    ecs->AddSystem<PickupSystem>();
    ecs->AddSystem<HealthChangeSystem>();
    ecs->AddSystem<HealthDisplaySystem>();
    ecs->AddSystem<CameraFollowPlayerSystem>();
    ecs->AddSystem<CameraUpdateSystem>(&viewMatrix, &projectionMatrix, &invPvMatrix, &uiProjectionMatrix);
    ecs->AddSystem<MouseFollowSystem>(&invPvMatrix);
    ecs->AddSystem<FrameCountSystem>();
    ecs->AddSystem<DebugSystem>();
    ecs->AddSystem<AnimationSystem>();
    ecs->AddSystem<RenderSystem>(renderer.get(), descriptorSetRenderer.get(), &commandBuffer, &viewMatrix, &projectionMatrix); // better way to store the RenderSystem data?
    ecs->AddSystem<UiRenderSystem>(uiRenderer.get(), uiDescriptorSetRenderer.get(), &commandBuffer, &uiProjectionMatrix);
    ecs->SetComponentListener<HealthComponentListener>();

    // TODO: Load from scene file
    for (int y = 0; y < 20; y++)
    {
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-10.0f, -10.0f + y * 1.0}, glm::vec2{1.0f, 1.0f});
        if(y == 0 || y == 19)
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{0.25f, 1.0f});
        else
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.75f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
        entity.AddComponent<Renderable>();
      }
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{10.0f, -10.0f + y * 1.0}, glm::vec2{1.0f, 1.0f});
        if(y == 0 || y == 19)
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.5f, 0.0f}, glm::vec2{0.75f, 1.0f});
        else
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.75f, 0.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
        entity.AddComponent<Renderable>();
      }
    }
    for (int x = 1; x < 20; x++)
    {
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.0, -10.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.25f, 0.0f}, glm::vec2{0.5f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
        entity.AddComponent<Renderable>();
      }
      {
        Entity entity = Entity::Create(ecs.get());
        entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.0, 10.0f}, glm::vec2{1.0f, 1.0f});
        entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.25f, 0.0f}, glm::vec2{0.5f, 1.0f});
        entity.AddComponent<StaticColliderC>(true);
        entity.AddComponent<Renderable>();
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
        entity.AddComponent<Renderable>();
      }
    }

    float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;
    Entity entityFox = Entity::Create(ecs.get());
    entityFox.AddComponent<TransformC>(glm::vec2{-0.9f, -0.4f}, glm::vec2{0.8f, 0.8f});
    entityFox.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityFox.AddComponent<StaticColliderC>(true);
    entityFox.AddComponent<Renderable>();

    Entity entityFontAtlas = Entity::Create(ecs.get());
    entityFontAtlas.AddComponent<TransformC>(glm::vec2{0.1f, -0.4f}, glm::vec2{0.8, 0.8});
    entityFontAtlas.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("font.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityFontAtlas.AddComponent<StaticColliderC>(false);
    entityFontAtlas.AddComponent<PickupC>();
    entityFontAtlas.AddComponent<Renderable>();

    Entity entityMouse = Entity::Create(ecs.get());
    entityMouse.AddComponent<TransformC>(glm::vec2(0.1), glm::vec2{0.2});
    entityMouse.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("fox.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    entityMouse.AddComponent<MouseFollowC>();
    entityMouse.AddComponent<Renderable>();

    Entity entityText = Entity::Create(ecs.get());
    entityText.AddComponent<TransformC>(glm::vec2{-aspect * 10.0f + 0.1f, 9.4f}, glm::vec2{1.0});
    entityText.AddComponent<TextC>(AssetRef{AssetManager::LoadAsset("font.meta")}, std::to_string(0) + " fps", 0.6f);
    entityText.AddComponent<FrameCountC>();
    entityText.AddComponent<Renderable>();

    Entity entityCamera = Entity::Create(ecs.get());
    entityCamera.AddComponent<CameraC>(BoundingBox(-aspect, -1.0f, aspect, 1.0f), false, false);
    entityCamera.AddComponent<TransformC>(glm::vec2{0.0f}, glm::vec2{2.0f});

    Entity entityUiCamera = Entity::Create(ecs.get());
    entityUiCamera.AddComponent<CameraC>(BoundingBox(0.0f, 0.0f, Vulkan::GetSwapChain().GetExtent().width, Vulkan::GetSwapChain().GetExtent().height), false, true);
    entityUiCamera.AddComponent<TransformC>(glm::vec2{0.0f}, glm::vec2{1.0f});

    Entity entityPlayer = Entity::Create(ecs.get());
    entityPlayer.AddComponent<PlayerC>(entityCamera, false);
    entityPlayer.AddComponent<HealthC>(10, 10);
    entityPlayer.AddComponent<PhysicsC>(0.1f, glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
    entityPlayer.AddComponent<TransformC>(glm::vec2{0.0f, 2.0f}, glm::vec2{1.0f});
    entityPlayer.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("character.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{0.25f, 1.0f});
    entityPlayer.AddComponent<AnimationC>(4, 4, 0, 3, 4, true, 0.5f);
    entityPlayer.AddComponent<DynamicColliderC>(false, glm::vec2{14.0f / 32.0f, 0.0f / 32.0f}, glm::vec2{4.0f / 32.0f, 21.0f / 32.0f}, glm::vec2{0.0f});
    entityPlayer.AddComponent<Renderable>();

    Entity entityDebug = Entity::Create(ecs.get());
    entityDebug.AddComponent<DebugC>(entityPlayer);
    entityDebug.AddComponent<TextC>(AssetRef(AssetManager::LoadAsset("font.meta")), "", 20.0f);
    entityDebug.AddComponent<TransformC>(glm::vec2{10.0f, Vulkan::GetSwapChain().GetExtent().height - 10.0f}, glm::vec2{1.0f});
    entityDebug.AddComponent<UiRenderable>();
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