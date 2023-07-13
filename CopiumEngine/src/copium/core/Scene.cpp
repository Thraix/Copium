#include "copium/core/Scene.h"

#include "copium/asset/AssetManager.h"
#include "copium/asset/AssetMeta.h"
#include "copium/asset/AssetRef.h"
#include "copium/core/Vulkan.h"
#include "copium/ecs/Entity.h"
#include "copium/ecs/System.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/example/AnimationSystem.h"
#include "copium/example/CameraFollowPlayerSystem.h"
#include "copium/example/CameraUpdateSystem.h"
#include "copium/example/ColliderSystem.h"
#include "copium/example/Components.h"
#include "copium/example/DebugSystem.h"
#include "copium/example/FrameCountSystem.h"
#include "copium/example/HealthChangeSystem.h"
#include "copium/example/HealthComponentListener.h"
#include "copium/example/HealthDisplaySystem.h"
#include "copium/example/LevelGeneratorComponentListener.h"
#include "copium/example/MouseFollowSystem.h"
#include "copium/example/PhysicsSystem.h"
#include "copium/example/PickupSystem.h"
#include "copium/example/PlayerControllerSystem.h"
#include "copium/example/RenderSystem.h"
#include "copium/example/UiRenderSystem.h"
#include "copium/event/ViewportResize.h"

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
    ecs->AddSystem<DebugSystem>(&viewport);
    ecs->AddSystem<AnimationSystem>();
    ecs->AddSystem<RenderSystem>(renderer.get(), descriptorSetRenderer.get(), &commandBuffer, &viewMatrix, &projectionMatrix); // better way to store the RenderSystem data?
    ecs->AddSystem<UiRenderSystem>(uiRenderer.get(), uiDescriptorSetRenderer.get(), &commandBuffer, &uiProjectionMatrix);
    ecs->SetComponentListener<HealthComponentListener>();
    ecs->SetComponentListener<LevelGeneratorComponentListener>();

    Deserialize("res/scenes/scene.meta");
  }

  void Scene::Update()
  {
    ecs->UpdateSystems();
  }

  EventResult Scene::OnEvent(const Event& event)
  {
    switch (event.GetType())
    {
    case EventType::ViewportResize:
      const ViewportResize& viewportResizeEvent = static_cast<const ViewportResize&>(event);
      viewport = viewportResizeEvent.GetViewport();
      break;
    }
    Input::PushViewport(viewport);
    ecs->UpdateSystems(EventSignal{event});
    Input::PopViewport();
    return EventResult::Continue;
  }

  void Scene::Deserialize(const std::string& file)
  {
    float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;

    std::vector<MetaFile> metaFiles = MetaFile::ReadList(file);

    for (auto& metaFile : metaFiles)
    {
      Entity entity = Entity::Create(ecs.get());
      for (auto& [name, metaClass] : metaFile.GetMetaFileClasses())
      {
        try
        {
          char* endPtr;
          if (name == "Transform")
          {
            TransformC transform;
            transform.position = ReadVec2Opt(metaClass, "position", glm::vec2{0.0f, 0.0f});
            transform.size = ReadVec2Opt(metaClass, "size", glm::vec2{1.0f, 1.0f});
            entity.AddComponent<TransformC>(transform);
          }
          else if (name == "Texture")
          {
            TextureC texture;
            texture.asset = AssetRef{AssetManager::LoadAsset(Uuid{metaClass.GetValue("texture-uuid")})};
            texture.texCoord1 = ReadVec2Opt(metaClass, "tex-coord", glm::vec2{0.0f, 0.0f});
            texture.texCoord2 = texture.texCoord1 + ReadVec2Opt(metaClass, "tex-size", glm::vec2{1.0f, 1.0f} - texture.texCoord1);
            entity.AddComponent<TextureC>(texture);
          }
          else if (name == "StaticCollider")
          {
            StaticColliderC staticCollider;
            staticCollider.resolveCollision = ReadBoolOpt(metaClass, "resolve-collision", true);
            entity.AddComponent<StaticColliderC>(staticCollider);
          }
          else if (name == "DynamicCollider")
          {
            DynamicColliderC dynamicCollider;
            dynamicCollider.resolveCollision = ReadBoolOpt(metaClass, "resolve-collision", true);
            dynamicCollider.colliderOffset = ReadVec2Opt(metaClass, "collider-offset", glm::vec2{0.0f, 0.0f});
            dynamicCollider.colliderSize = ReadVec2Opt(metaClass, "collider-size", glm::vec2{1.0f, 1.0f});
            entity.AddComponent<DynamicColliderC>(dynamicCollider);
          }
          else if (name == "Text")
          {
            TextC text;
            text.font = AssetRef{AssetManager::LoadAsset(Uuid{metaClass.GetValue("font")})};
            text.text = metaClass.GetValue("text");
            text.fontSize = std::strtof(metaClass.GetValue("font-size").c_str(), &endPtr);
            entity.AddComponent<TextC>(text);
          }
          else if (name == "Camera")
          {
            CameraC camera;
            camera.staticBoundingBox = ReadBoolOpt(metaClass, "static-bounding-box", false);
            camera.uiCamera = ReadBoolOpt(metaClass, "ui-camera", false);
            if (camera.uiCamera)
              camera.projection = BoundingBox(0, 0, Vulkan::GetSwapChain().GetExtent().width, Vulkan::GetSwapChain().GetExtent().height);
            else
              camera.projection = BoundingBox(-aspect, -1.0f, aspect, 1.0f);
            entity.AddComponent<CameraC>(camera);
          }
          else if (name == "Uuid")
          {
            UuidC uuid;
            uuid.uuid = Uuid{metaClass.GetValue("uuid")};
            entity.AddComponent<UuidC>(uuid);
          }
          else if (name == "Player")
          {
            PlayerC player;
            player.camera = GetEntity(Uuid{metaClass.GetValue("camera-uuid")});
            entity.AddComponent<PlayerC>(player);
          }
          else if (name == "Health")
          {
            HealthC health;
            health.max = std::strtol(metaClass.GetValue("health").c_str(), &endPtr, 10);
            health.current = health.max;
            entity.AddComponent<HealthC>(health);
          }
          else if (name == "Physics")
          {
            PhysicsC physics;
            physics.mass = std::strtof(metaClass.GetValue("mass").c_str(), &endPtr);
            entity.AddComponent<PhysicsC>(physics);
          }
          else if (name == "Animation")
          {
            AnimationC animation;
            animation.sheetCoord = ReadVec2Opt(metaClass, "sheet-coord", glm::ivec2{0, 0});
            animation.sheetSize = ReadVec2Opt(metaClass, "sheet-size", glm::ivec2{1, 1});
            animation.images = std::strtol(metaClass.GetValue("images").c_str(), &endPtr, 10);
            animation.horizontal = ReadBoolOpt(metaClass, "horizontal", true);
            animation.time = std::strtof(metaClass.GetValue("time").c_str(), &endPtr);
            entity.AddComponent<AnimationC>(animation);
          }
          else if (name == "Debug")
          {
            DebugC debug;
            debug.playerEntity = GetEntity(Uuid{metaClass.GetValue("player-uuid")});
            entity.AddComponent<DebugC>(debug);
          }
          else if (name == "Renderable")
            entity.AddComponent<RenderableC>();
          else if (name == "Pickup")
            entity.AddComponent<PickupC>();
          else if (name == "FrameCount")
            entity.AddComponent<FrameCountC>();
          else if (name == "MouseFollow")
            entity.AddComponent<MouseFollowC>();
          else if (name == "UiRenderable")
            entity.AddComponent<UiRenderableC>();
          else if(name == "LevelGenerator")
            entity.AddComponent<LevelGeneratorC>();
          else
            CP_WARN("Unknown component: %s", name.c_str());
        }
        catch (RuntimeException& exception) { CP_ERR("Invalid %s component: %s", name.c_str(), exception.GetErrorMessage().c_str()); }
      }
    }
  }

  Entity Scene::GetEntity(const Uuid& uuid) const
  {
    Entity entity{ecs.get(),ecs->Find<UuidC>([&](EntityId entity, const UuidC& uuidArg) { return uuid == uuidArg.uuid; })};
    CP_ASSERT(entity, "Failed to find entity with Uuid=%s", uuid.ToString().c_str());

    return entity;
  }

  glm::vec2 Scene::ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::vec2 vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    char* endPos;
    vec.x = std::strtof(value.c_str(), &endPos);
    vec.y = std::strtof(endPos, &endPos);
    return vec;
  }

  glm::ivec2 Scene::ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::ivec2 vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    char* endPos;
    vec.x = std::strtof(value.c_str(), &endPos);
    vec.y = std::strtof(endPos, &endPos);
    return vec;
  }


  bool Scene::ReadBoolOpt(const MetaFileClass& metaClass, const std::string& key, bool vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    if (value == "true")
      return true;
    return false;
  }
}