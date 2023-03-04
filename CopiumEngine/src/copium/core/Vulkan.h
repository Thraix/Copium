#pragma once

#include <memory>

namespace Copium
{
  class Instance;
  class Window;
  class Device;
  class SwapChain;

  class Vulkan
  {
  private:
    std::unique_ptr<Instance> instance;
    std::unique_ptr<Window> window;
    std::unique_ptr<Device> device;
    std::unique_ptr<SwapChain> swapChain;
  public:
    void SetInstance(std::unique_ptr<Instance>&& instance);
    void SetWindow(std::unique_ptr<Window>&& window);
    void SetDevice(std::unique_ptr<Device>&& device);
    void SetSwapChain(std::unique_ptr<SwapChain>&& swapChain);
    Instance& GetInstance() const;
    Window& GetWindow() const;
    Device& GetDevice() const;
    SwapChain& GetSwapChain() const;
    bool Valid();
 
  };
}