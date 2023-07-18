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
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>

namespace Copium
{
  Scene::Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool)
  {
    fileIcon = AssetManager::LoadAsset("fileicon.meta");
    descriptorSetFileIcon = Vulkan::GetImGuiInstance().CreateDescriptorSet();
    descriptorSetFileIcon->SetSampler(AssetManager::GetAsset<Texture2D>(fileIcon), 0, 0);
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

  Scene::~Scene()
  {
    AssetManager::UnloadAsset(fileIcon);
  }

  void Scene::Update()
  {
    EntityViewGui();
    ComponentViewGui();
    AssetViewGui();
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
      entity.AddComponent<SerializableC>();
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
          else if (name == "Name")
          {
            NameC name;
            name.name = metaClass.GetValue("name");
            entity.AddComponent<NameC>(name);
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
          else if (name == "LevelGenerator")
            entity.AddComponent<LevelGeneratorC>();
          else
            CP_WARN("Unknown component: %s", name.c_str());
        }
        catch (RuntimeException& exception) { CP_ERR("Invalid %s component: %s", name.c_str(), exception.GetErrorMessage().c_str()); }
      }
      if (!entity.HasComponent<UuidC>())
        entity.AddComponent<UuidC>();
      if (!entity.HasComponent<NameC>())
        entity.AddComponent<NameC>(String::Format("Entity %d", entity.GetId()));
    }
  }

  void Scene::EntityViewGui()
  {
    ImGui::Begin("Entity Tree View");
    ecs->Each<SerializableC>([&](EntityId entityId, SerializableC& serializable) {
      Entity entity{ecs.get(), entityId};
    std::string name;
    if (entity.HasComponent<NameC>())
      name = entity.GetComponent<NameC>().name;
    if (name.empty())
      name = String::Format("Entity %u", entity.GetId());
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (selectedEntity == entity)
      flags |= ImGuiTreeNodeFlags_Selected;
    ImGui::TreeNodeEx(name.c_str(), flags);
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
      selectedEntity = entity;
      });
    ImGui::End();
  }

  void Scene::ComponentViewGui()
  {
    ImGui::Begin("Entitiy View");
    if (selectedEntity)
    {
      ImGui::SeparatorText("Entity");
      ImGui::Text("Uuid: %s", selectedEntity.GetComponent<UuidC>().uuid.ToString().c_str());
      if (ImGui::Button("Delete Entity"))
      {
        selectedEntity.Destroy();
        selectedEntity.Invalidate();
      }

      ImGui::SeparatorText("Components");
      ComponentGui<NameC>(selectedEntity, "Name", Scene::NameGui, Scene::NameCreate);
      ComponentGui<TransformC>(selectedEntity, "Transform", Scene::TransformGui, Scene::TransformCreate);
      ComponentGui<TextureC>(selectedEntity, "Texture", Scene::TextureGui, Scene::TextureCreate);
      ComponentGui<TextC>(selectedEntity, "Text", Scene::TextGui, Scene::TextCreate);
      ComponentGui<PlayerC>(selectedEntity, "Player", Scene::PlayerGui, Scene::PlayerCreate);
      ComponentGui<StaticColliderC>(selectedEntity, "Static Collider", Scene::StaticColliderGui, Scene::StaticColliderCreate);
      ComponentGui<DynamicColliderC>(selectedEntity, "Dynamic Collider", Scene::DynamicColliderGui, Scene::DynamicColliderCreate);
      ComponentGui<CameraC>(selectedEntity, "Camera", Scene::CameraGui, Scene::CameraCreate);
      ComponentGui<HealthC>(selectedEntity, "Health", Scene::HealthGui, Scene::HealthCreate);
      ComponentGui<PhysicsC>(selectedEntity, "Physics", Scene::PhysicsGui, Scene::PhysicsCreate);
      ComponentGui<AnimationC>(selectedEntity, "Animation", Scene::AnimationGui, Scene::AnimationCreate);
      ComponentGui<DebugC>(selectedEntity, "Debug Information", Scene::DebugGui, Scene::DebugCreate);

      ComponentGui<MouseFollowC>(selectedEntity, "Mouse Follow");
      ComponentGui<RenderableC>(selectedEntity, "Renderable");
      ComponentGui<UiRenderableC>(selectedEntity, "Ui Renderable");
      ComponentGui<PickupC>(selectedEntity, "Pickup");
      ComponentGui<LevelGeneratorC>(selectedEntity, "Level Generator");
      ComponentGui<FrameCountC>(selectedEntity, "Frame Count");
    }
    ImGui::End();
  }

  void Scene::AssetViewGui()
  {
    ImGui::Begin("Asset View");
    std::vector<AssetFile> assetFiles = AssetManager::GetAssetFiles();
    float width = 0;
    float maxWidth = ImGui::GetWindowContentRegionMax().x;
    ImGuiStyle& style = ImGui::GetStyle();
    float defaultSpacing = style.ItemSpacing.x;
    style.ItemSpacing.x = 30;
    float buttonWidth = 50;
    float itemWidth = buttonWidth + style.ItemSpacing.x;
    for (int i = 0; i < assetFiles.size(); i++)
    {
      std::string name = assetFiles[i].GetPath();
      size_t slash = name.find('/');
      if (slash != std::string::npos)
        name = name.substr(slash + 1);
      ImGui::BeginGroup();
      ImGui::Image(*descriptorSetFileIcon, ImVec2{buttonWidth, buttonWidth}, ImVec2{0, 1}, ImVec2{1, 0});

      ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + buttonWidth);
      ImGui::TextWrapped(name.c_str());
      ImGui::PopTextWrapPos();
      ImGui::EndGroup();
      width += itemWidth;
      if (i < assetFiles.size() - 1 && width + itemWidth >= maxWidth)
        width = 0;
      else
        ImGui::SameLine();
    }
    style.ItemSpacing.x = defaultSpacing;
    ImGui::End();
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

  template <typename Component, typename FuncGui, typename FuncCreate>
  void Scene::ComponentGui(Entity entity, const std::string& componentName, FuncGui funcGui, FuncCreate funcCreate)
  {
    if (entity.HasComponent<Component>())
    {
      Component& component = entity.GetComponent<Component>();
      if (ImGui::CollapsingHeader(componentName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
      {
        funcGui(component);
        if (ImGui::Button(std::string("Delete " + componentName).c_str()))
          entity.RemoveComponent<Component>();
      }
    }
    else
    {
      // TODO: These buttons should probably be in a context menu when you right-click the entity instead
      if (ImGui::Button(std::string("Add " + componentName).c_str()))
        funcCreate(entity);
    }
  }

  template <typename Component>
  void Scene::ComponentGui(Entity entity, const std::string& componentName)
  {
    if (entity.HasComponent<Component>())
    {
      Component& component = entity.GetComponent<Component>();
      if (ImGui::Button(std::string("Delete " + componentName).c_str()))
        entity.RemoveComponent<Component>();
    }
    else
    {
      // TODO: These buttons should probably be in a context menu when you right-click the entity instead
      if (ImGui::Button(std::string("Add " + componentName).c_str()))
        entity.AddComponent<Component>();
    }
  }

  void Scene::TransformGui(TransformC& transform)
  {
    ImGui::DragFloat2("Position", (float*)&transform.position);
    ImGui::DragFloat2("Size", (float*)&transform.size);
  }

  void Scene::TransformCreate(Entity entity)
  {
    entity.AddComponent<TransformC>(glm::vec2{0, 0}, glm::vec2{1, 1});
  }

  void Scene::TextureGui(TextureC& texture)
  {
    Asset& asset = AssetManager::GetAsset<Texture2D>(texture.asset);
    ImGui::Text("Asset: %s", asset.GetName().c_str());
    ImGui::DragFloat2("Tex Coord 1", (float*)&texture.texCoord1, 0.01, 0.0f, 1.0f);
    ImGui::DragFloat2("Tex Coord 2", (float*)&texture.texCoord2, 0.01, 0.0f, 1.0f);
  }

  void Scene::TextureCreate(Entity entity)
  {
    entity.AddComponent<TextureC>(AssetRef{AssetManager::DuplicateAsset(Vulkan::GetEmptyTexture2D())}, glm::vec2{0, 0}, glm::vec2{1, 1});
  }

  void Scene::TextGui(TextC& text)
  {
    Asset& asset = AssetManager::GetAsset<Font>(text.font);
    ImGui::Text(asset.GetName().c_str());
    ImGui::InputTextMultiline("Text##Text", &text.text);
    ImGui::DragFloat("Font Size", &text.fontSize);
  }

  void Scene::TextCreate(Entity entity)
  {
    entity.AddComponent<TextC>(AssetRef{AssetManager::LoadAsset<Font>("font.meta")}, "", 20.0f);
  }

  void Scene::PlayerGui(PlayerC& player)
  {
    if (player.camera)
      ImGui::Text("Camera: %s", player.camera.GetComponent<NameC>().name.c_str());
    else
      ImGui::Text("No camera attached");
  }

  void Scene::PlayerCreate(Entity entity)
  {
    entity.AddComponent<PlayerC>();
  }

  void Scene::StaticColliderGui(StaticColliderC& staticCollider)
  {
    ImGui::Checkbox("StaticCollider##Resolve Collision", &staticCollider.resolveCollision);
  }

  void Scene::StaticColliderCreate(Entity entity)
  {
    entity.AddComponent<StaticColliderC>(false);
  }

  void Scene::DynamicColliderGui(DynamicColliderC& dynamicCollider)
  {
    ImGui::Checkbox("DynamicCollider##Resolve Collision", &dynamicCollider.resolveCollision);
    ImGui::DragFloat2("Collider Offset", (float*)&dynamicCollider.colliderOffset, 0.01);
    ImGui::DragFloat2("Collider Size", (float*)&dynamicCollider.colliderSize, 0.01);
  }

  void Scene::DynamicColliderCreate(Entity entity)
  {
    entity.AddComponent<DynamicColliderC>(true, glm::vec2{0, 0}, glm::vec2{1, 1});
  }

  void Scene::CameraGui(CameraC& camera)
  {
    ImGui::Checkbox("Static", &camera.staticBoundingBox);
    ImGui::Checkbox("Ui camera", &camera.uiCamera); // TODO: If this changes, the bounding box should be modified if it is not static
  }

  void Scene::CameraCreate(Entity entity)
  {
    // TODO: Make the BoundingBox based on the viewport somehow
    entity.AddComponent<CameraC>(BoundingBox{-1.0f, -1.0f, 1.0f, 1.0f}, false, false);
  }

  void Scene::HealthGui(HealthC& health)
  {
    ImGui::DragInt("Max Health", &health.max);
  }

  void Scene::HealthCreate(Entity entity)
  {
    entity.AddComponent<HealthC>(10, 10);
  }

  void Scene::PhysicsGui(PhysicsC& physics)
  {
    ImGui::DragFloat("Mass", &physics.mass);
  }

  void Scene::PhysicsCreate(Entity entity)
  {
    entity.AddComponent<PhysicsC>(10.0f);
  }

  void Scene::AnimationGui(AnimationC& animation)
  {
    ImGui::DragInt2("Sheet Size", (int*)&animation.sheetSize);
    ImGui::DragInt2("Sheet Coord", (int*)&animation.sheetCoord, 1, 0, std::max(animation.sheetSize.x, animation.sheetSize.y));
    ImGui::DragInt("Images", &animation.images);
    ImGui::DragFloat("Frame time", &animation.time);
    ImGui::Checkbox("Horizontal", &animation.horizontal);
  }

  void Scene::AnimationCreate(Entity entity)
  {
    entity.AddComponent<AnimationC>(glm::ivec2{0, 0}, glm::ivec2{1, 1}, 1, true, 1.0f);
  }

  void Scene::DebugGui(DebugC& debug)
  {
    if (debug.playerEntity)
      ImGui::Text("Player: %s", debug.playerEntity.GetComponent<NameC>().name.c_str());
    else
      ImGui::Text("No player attached");
  }

  void Scene::DebugCreate(Entity entity)
  {
    entity.AddComponent<DebugC>();
  }

  void Scene::NameGui(NameC& name)
  {
    ImGui::InputText("Name##Name", &name.name);
  }

  void Scene::NameCreate(Entity entity)
  {
    entity.AddComponent<NameC>(String::Format("Entity %d", entity.GetId()));
  }
}