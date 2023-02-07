#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <set>

#include "DebugMessenger.h"
#include "QueueFamilies.h"
#include "SwapChain.h"
#include "Timer.h"

namespace Copium
{
  class Instance final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Instance);
  private:
    static const int MAX_FRAMES_IN_FLIGHT = 2;
    static const int WINDOW_WIDTH = 1920;
    static const int WINDOW_HEIGHT = 1080;

    VkInstance instance;
    GLFWwindow* window;
    VkSurfaceKHR surface;
    std::unique_ptr<DebugMessenger> debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    std::unique_ptr<SwapChain> swapChain;
    int flightIndex;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    VkCommandPool commandPool;
    bool framebufferResized = false;

    int frameCount = 0;
    Timer timer;

  public:
    Instance(const std::string& applicationName);
    ~Instance();
    bool BeginPresent();
    bool EndPresent();
    void SubmitGraphicsQueue(const std::vector<VkCommandBuffer>& commandBuffers);
    VkInstance GetInstance() const;
    GLFWwindow* GetWindow() const;
    VkSurfaceKHR GetSurface() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    VkCommandPool GetCommandPool() const;
    VkQueue GetGraphicsQueue() const;
    int GetFlightIndex() const;
    int GetMaxFramesInFlight() const;
    const SwapChain& GetSwapChain() const;
    // TODO: Create Device class and move this there
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

  private:
    void InitializeWindow(const std::string& applicationName);
    void InitializeInstance(const std::string& applicationName);
    void InitializeDebugMessenger();
    void InitializeSurface();
    void SelectPhysicalDevice();
    void InitializeLogicalDevice();
    void InitializeSwapChain();
    void InitializeCommandPool();
    void InitializeSyncObjects();
    std::vector<const char*> GetRequiredExtensions();
    bool CheckLayerSupport(const std::vector<const char*>& layers);
    bool IsPhysicalDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> GetRequiredDeviceExtensions();
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

  };
}
