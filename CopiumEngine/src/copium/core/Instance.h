#pragma once

#include "copium/core/DebugMessenger.h"
#include "copium/util/Timer.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <set>

namespace Copium
{
  class Instance final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Instance);
  private:
    VkInstance instance;
    std::unique_ptr<DebugMessenger> debugMessenger;

    // TODO: Move to SwapChain?
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    // TODO end

    Timer timer;

  public:
    Instance(const std::string& applicationName);
    ~Instance();

    operator VkInstance() const;

  private:
    void InitializeInstance(const std::string& applicationName);
    void InitializeDebugMessenger();
    std::vector<const char*> GetRequiredExtensions();
    bool CheckLayerSupport(const std::vector<const char*>& layers);
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  };
}
