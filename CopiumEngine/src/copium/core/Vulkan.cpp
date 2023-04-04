#include "copium/core/Vulkan.h"

namespace Copium
{
  std::unique_ptr<Instance> Vulkan::instance;
  std::unique_ptr<Window> Vulkan::window;
  std::unique_ptr<Device> Vulkan::device;
  std::unique_ptr<SwapChain> Vulkan::swapChain;

  void Vulkan::Initialize()
  {
    instance = std::make_unique<Instance>("Copium Engine");
    window = std::make_unique<Window>( "Copium Engine", 1920, 1080, Window::Mode::Windowed);
    device = std::make_unique<Device>();
    swapChain = std::make_unique<SwapChain>();
  }

  void Vulkan::Destroy()
  {
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

  bool Vulkan::Valid()
  {
    return instance && window && device && swapChain;
  }
}