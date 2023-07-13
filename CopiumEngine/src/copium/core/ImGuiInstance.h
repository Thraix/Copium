#pragma once

#include <memory>

#include "copium/buffer/CommandBuffer.h"
#include "copium/pipeline/DescriptorPool.h"

namespace Copium
{
  class ImGuiInstance
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ImGuiInstance);

  private:
    std::unique_ptr<DescriptorPool> descriptorPool;

  public:
    ImGuiInstance();
    ~ImGuiInstance();

    void Begin();
    void End();
    void Render(CommandBuffer& commandBuffer);

  private:
    static void CheckVkResult(VkResult err);
  };
}
