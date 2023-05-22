#pragma once

#include "copium/renderer/Renderer.h"
#include "copium/ecs/ECSManager.h"
#include "copium/event/Event.h"
#include "copium/event/EventResult.h"

#include <memory>

namespace Copium
{
  class Scene
  {
  private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<ECSManager> ecs;
    std::unique_ptr<DescriptorSet> descriptorSetRenderer;
    int frameCounter = 0;
    int fps = 0;
  public:
    Scene(CommandBuffer& commandBuffer, DescriptorPool& descriptorPool);
    void Update();
    EventResult OnEvent(const Event& event);
  };
}