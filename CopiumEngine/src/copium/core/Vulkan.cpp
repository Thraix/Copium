#include "copium/core/Vulkan.h"

#include "copium/asset/AssetManager.h"
#include "copium/buffer/Framebuffer.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/sampler/ColorAttachment.h"
#include "copium/sampler/Font.h"
#include "copium/sampler/Texture2D.h"
#include "copium/util/Timer.h"

namespace Copium
{
  std::unique_ptr<Instance> Vulkan::instance;
  std::unique_ptr<Window> Vulkan::window;
  std::unique_ptr<Device> Vulkan::device;
  std::unique_ptr<SwapChain> Vulkan::swapChain;
  std::unique_ptr<ImGuiInstance> Vulkan::imGuiInstance;
  AssetHandle<Texture2D> Vulkan::emptyTexture2D;
  AssetHandle<Texture2D> Vulkan::whiteTexture2D;

  void Vulkan::Initialize()
  {
    Timer timer;
    timer.Start();

    glfwSetErrorCallback(glfw_error_callback);
    CP_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize the glfw context");

    instance = std::make_unique<Instance>("Copium Engine");
    window = std::make_unique<Window>("Copium Engine", 1440, 810, WindowMode::Windowed);
    device = std::make_unique<Device>();
    swapChain = std::make_unique<SwapChain>();
    imGuiInstance = std::make_unique<ImGuiInstance>();
    CP_INFO("Initialized Vulkan in %f seconds", timer.Elapsed());

    timer.Start();
    AssetManager::RegisterAssetType<Texture2D>("Texture2D");
    AssetManager::RegisterAssetType<ColorAttachment>("RenderTexture");
    AssetManager::RegisterAssetType<Pipeline>("Pipeline");
    AssetManager::RegisterAssetType<Framebuffer>("Framebuffer");
    AssetManager::RegisterAssetType<Font>("Font");

    // TODO: Make the working directory always be relative to the assets folder
    //       By looking at where the executable is, since that should always be in the bin folder (it currently isn't though)
    AssetManager::RegisterAssetDir("assets/");
    emptyTexture2D = AssetHandle<Texture2D>{"empty_texture2d", std::make_unique<Texture2D>(std::vector<uint8_t>{255, 0, 255, 255}, 1, 1, SamplerCreator{})};
    whiteTexture2D = AssetHandle<Texture2D>{"white_texture2d", std::make_unique<Texture2D>(std::vector<uint8_t>{255, 255, 255, 255}, 1, 1, SamplerCreator{})};
    CP_INFO("Initialized AssetManager in %f seconds", timer.Elapsed());
  }

  void Vulkan::Destroy()
  {
    emptyTexture2D.UnloadAsset();
    whiteTexture2D.UnloadAsset();
    AssetManager::UnregisterAssetDir("assets/");
    AssetManager::Cleanup();
    imGuiInstance.reset();
    device->WaitIdle();
    swapChain.reset();
    device->CleanupIdleQueue();
    device.reset();
    window.reset();
    instance.reset();
  }

  Instance& Vulkan::GetInstance()
  {
    return *instance;
  }

  Window& Vulkan::GetWindow()
  {
    return *window;
  }

  Device& Vulkan::GetDevice()
  {
    return *device;
  }

  SwapChain& Vulkan::GetSwapChain()
  {
    return *swapChain;
  }

  ImGuiInstance& Vulkan::GetImGuiInstance()
  {
    return *imGuiInstance;
  }

  AssetHandle<Texture2D> Vulkan::GetWhiteTexture2D()
  {
    return whiteTexture2D;
  }

  AssetHandle<Texture2D> Vulkan::GetEmptyTexture2D()
  {
    return emptyTexture2D;
  }

  bool Vulkan::Valid()
  {
    return instance && window && device && swapChain;
  }

  void Vulkan::glfw_error_callback(int error, const char* description)
  {
    CP_ABORT("GLFW Error %d: %s\n", error, description);
  }
}
