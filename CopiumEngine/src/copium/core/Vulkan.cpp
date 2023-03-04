#include "copium/core/Vulkan.h"

#include "copium/core/Device.h"
#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"
#include "copium/core/Window.h"

namespace Copium
{
  void Vulkan::SetInstance(std::unique_ptr<Instance>&& instance)
  {
    this->instance = std::move(instance);
  }

  void Vulkan::SetWindow(std::unique_ptr<Window>&& window)
  {
    this->window = std::move(window);
  }

  void Vulkan::SetDevice(std::unique_ptr<Device>&& device)
  {
    this->device = std::move(device);
  }

  void Vulkan::SetSwapChain(std::unique_ptr<SwapChain>&& swapChain)
  {
    this->swapChain = std::move(swapChain);
  }

  Instance& Vulkan::GetInstance() const
  {
    return *instance;
  }

  Window& Vulkan::GetWindow() const
  {
    return *window;
  }

  Device& Vulkan::GetDevice() const
  {
    return *device;
  }

  SwapChain& Vulkan::GetSwapChain() const
  {
    return *swapChain;
  }

  bool Vulkan::Valid()
  {
    return instance && window && device && swapChain;
  }
}