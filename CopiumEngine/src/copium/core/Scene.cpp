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
#include "copium/example/ComponentHandlers.h"
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
    : viewport{-1, -1, 1, 1}
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

    RegisterComponentHandler<NameComponentHandler>();
    RegisterComponentHandler<TransformComponentHandler>();
    RegisterComponentHandler<TextureComponentHandler>();
    RegisterComponentHandler<TextComponentHandler>();
    RegisterComponentHandler<StaticColliderComponentHandler>();
    RegisterComponentHandler<DynamicColliderComponentHandler>();
    RegisterComponentHandler<PlayerComponentHandler>();
    RegisterComponentHandler<CameraComponentHandler>(&viewport);
    RegisterComponentHandler<UuidComponentHandler>();
    RegisterComponentHandler<PhysicsComponentHandler>();
    RegisterComponentHandler<HealthComponentHandler>();
    RegisterComponentHandler<AnimationComponentHandler>();
    RegisterComponentHandler<DebugComponentHandler>();
    RegisterFlagComponentHandler<MouseFollowC>("Mouse Follower");
    RegisterFlagComponentHandler<RenderableC>("Renderable");
    RegisterFlagComponentHandler<UiRenderableC>("Ui Renderable");
    RegisterFlagComponentHandler<PickupC>("Pickup");
    RegisterFlagComponentHandler<LevelGeneratorC>("Level Generator");
    RegisterFlagComponentHandler<FrameCountC>("Frame Counter");

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
    std::vector<MetaFile> metaFiles = MetaFile::ReadList(file);

    for (auto& metaFile : metaFiles)
    {
      Entity entity = Entity::Create(ecs.get());
      entity.AddComponent<SerializableC>();
      for (auto& [name, metaClass] : metaFile.GetMetaFileClasses())
      {
        try
        {
          bool found = false;
          for (auto& componentHandler : componentHandlers)
          {
            if (name == componentHandler->GetSerializedName())
            {
              componentHandler->Deserialize(entity, metaClass);
              found = true;
              break;
            }
          }
          if (!found)
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

      for (auto& componentHandler : componentHandlers)
      {
        componentHandler->ComponentGui(selectedEntity);
      }
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
}