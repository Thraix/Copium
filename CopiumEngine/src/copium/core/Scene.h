#pragma once

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/Entity.h"
#include "copium/event/Event.h"
#include "copium/event/EventResult.h"
#include "copium/example/Components.h"
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

    Entity GetEntity(const Uuid& uuid) const;

    glm::vec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::vec2 vec);
    glm::ivec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::ivec2 vec);
    bool ReadBoolOpt(const MetaFileClass& metaClass, const std::string& key, bool vec);

    template <typename Component, typename FuncGui, typename FuncCreate>
    void ComponentGui(Entity entity, const std::string& componentName, FuncGui funcGui, FuncCreate funcCreate);
    template <typename Component>
    static void ComponentGui(Entity entity, const std::string& componentName);

    // TODO: Make these a class instead?
    static void TransformGui(TransformC& transform);
    static void TransformCreate(Entity entity);
    static void TextureGui(TextureC& texture);
    static void TextureCreate(Entity entity);
    static void TextGui(TextC& text);
    static void TextCreate(Entity entity);
    static void PlayerGui(PlayerC& player);
    static void PlayerCreate(Entity entity);
    static void StaticColliderGui(StaticColliderC& staticCollider);
    static void StaticColliderCreate(Entity entity);
    static void DynamicColliderGui(DynamicColliderC& dynamicCollider);
    static void DynamicColliderCreate(Entity entity);
    static void CameraGui(CameraC& camera);
    static void CameraCreate(Entity entity);
    static void PhysicsGui(PhysicsC& physics);
    static void PhysicsCreate(Entity entity);
    static void HealthGui(HealthC& health);
    static void HealthCreate(Entity entity);
    static void AnimationGui(AnimationC& animation);
    static void AnimationCreate(Entity entity);
    static void DebugGui(DebugC& debug);
    static void DebugCreate(Entity entity);
    static void NameGui(NameC& name);
    static void NameCreate(Entity entity);
  };
}