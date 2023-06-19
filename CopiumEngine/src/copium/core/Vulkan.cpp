#include "copium/core/Vulkan.h"

#include "copium/asset/AssetManager.h"
#include "copium/sampler/Texture2D.h"
#include "copium/sampler/ColorAttachment.h"
#include "copium/sampler/Font.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/buffer/Framebuffer.h"

namespace Copium
{
  std::unique_ptr<Instance> Vulkan::instance;
  std::unique_ptr<Window> Vulkan::window;
  std::unique_ptr<Device> Vulkan::device;
  std::unique_ptr<SwapChain> Vulkan::swapChain;
  AssetHandle Vulkan::emptyTexture2D;

  void Vulkan::Initialize()
  {
    instance = std::make_unique<Instance>("Copium Engine");
    window = std::make_unique<Window>("Copium Engine", 1440, 810, WindowMode::Windowed);
    device = std::make_unique<Device>();
    swapChain = std::make_unique<SwapChain>();

    AssetManager::RegisterAssetType<Texture2D>("Texture2D");
    AssetManager::RegisterAssetType<ColorAttachment>("RenderTexture");
    AssetManager::RegisterAssetType<Pipeline>("Pipeline");
    AssetManager::RegisterAssetType<Framebuffer>("Framebuffer");
    AssetManager::RegisterAssetType<Font>("Font");

    // TODO: Make the working directory always be relative to the assets folder
    //       By looking at where the executable is, since that should always be in the bin folder (it currently isn't though)
    AssetManager::RegisterAssetDir("assets/");
    emptyTexture2D = AssetManager::RegisterRuntimeAsset("empty_texture2d", std::make_unique<Texture2D>(std::vector<uint8_t>{0, 0, 0, 255}, 1, 1, SamplerCreator{}));
  }

  void Vulkan::Destroy()
  {
    AssetManager::UnloadAsset(emptyTexture2D);
    AssetManager::UnregisterAssetDir("assets/");
    AssetManager::Cleanup();
    swapChain.reset();
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

  AssetHandle Vulkan::GetEmptyTexture2D()
  {
    return emptyTexture2D;
  }

  bool Vulkan::Valid()
  {
    return instance && window && device && swapChain;
  }
}