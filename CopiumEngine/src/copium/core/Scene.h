#pragma once

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/Entity.h"
#include "copium/event/Event.h"
#include "copium/event/EventResult.h"
#include "copium/example/Components.h"
#include "copium/example/ComponentHandlerBase.h"
#include "copium/example/ComponentHandler.h"
#include "copium/renderer/Renderer.h"
#include "copium/util/Uuid.h"

#include <memory>

namespace Copium
{
  class Scene
  {
  private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Renderer> uiRenderer;
    std::unique_ptr<ECSManager> ecs;
    std::unique_ptr<DescriptorSet> descriptorSetRenderer;
    std::unique_ptr<DescriptorSet> uiDescriptorSetRenderer;
    std::vector<std::unique_ptr<ComponentHandlerBase>> componentHandlers;
    AssetHandle fileIcon;
    std::unique_ptr<DescriptorSet> descriptorSetFileIcon;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 invPvMatrix;
    glm::mat4 uiProjectionMatrix;
    BoundingBox viewport;

    Entity selectedEntity;
  public:
    Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool);
    ~Scene();
    void Update();
    EventResult OnEvent(const Event& event);
  private:
    void Deserialize(const std::string& file);
    void EntityViewGui();
    void ComponentViewGui();
    void AssetViewGui();

    template <typename ComponentHandler, typename... Args>
    void RegisterComponentHandler(const Args&... args)
    {
      componentHandlers.emplace_back(std::make_unique<ComponentHandler>(args...));
    }

    template <typename Component>
    void RegisterFlagComponentHandler(const std::string& name)
    {
      componentHandlers.emplace_back(std::make_unique<ComponentHandler<Component>>(name, true));
    }
  };
}