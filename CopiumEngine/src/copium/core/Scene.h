#pragma once

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/Entity.h"
#include "copium/event/Event.h"
#include "copium/event/EventResult.h"
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
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 invPvMatrix;
    glm::mat4 uiProjectionMatrix;
  public:
    Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool);
    void Update();
    EventResult OnEvent(const Event& event);
  private:
    void Deserialize(const std::string& file);
    Entity GetEntity(const Uuid& uuid) const;

    glm::vec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::vec2 vec);
    glm::ivec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::ivec2 vec);
    bool ReadBoolOpt(const MetaFileClass& metaClass, const std::string& key, bool vec);
  };
}