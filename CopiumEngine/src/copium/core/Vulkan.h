#pragma once

#include "copium/core/Device.h"
#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"
#include "copium/core/Window.h"
#include "copium/core/ImGuiInstance.h"
#include "copium/util/Common.h"

#include <memory>

namespace Copium
{
  class Vulkan
  {
    CP_STATIC_CLASS(Vulkan);
  private:
    static std::unique_ptr<Instance> instance;
    static std::unique_ptr<Window> window;
    static std::unique_ptr<Device> device;
    static std::unique_ptr<SwapChain> swapChain;
    static std::unique_ptr<ImGuiInstance> imGuiInstance;

    static AssetHandle emptyTexture2D;
  public:
    static void Initialize();
    static void Destroy();
    static Instance& GetInstance();
    static Window& GetWindow();
    static Device& GetDevice();
    static SwapChain& GetSwapChain();
    static ImGuiInstance& GetImGuiInstance();
    static bool Valid();
    static AssetHandle GetEmptyTexture2D();
  };
}