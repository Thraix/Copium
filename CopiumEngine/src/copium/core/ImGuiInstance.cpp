#include "ImGuiInstance.h"

#include "copium/core/Vulkan.h"
#include "copium/buffer/CommandBufferScoped.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Copium
{
  ImGuiInstance::ImGuiInstance()
    : descriptorPool{std::make_unique<DescriptorPool>()}
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(Vulkan::GetWindow().GetWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = Vulkan::GetInstance();
    init_info.PhysicalDevice = Vulkan::GetDevice().GetPhysicalDevice();
    init_info.Device = Vulkan::GetDevice();
    init_info.QueueFamily = Vulkan::GetDevice().GetGraphicsQueueFamily();
    init_info.Queue = Vulkan::GetDevice().GetGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = *descriptorPool.get();
    init_info.Subpass = 0;
    init_info.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount = Vulkan::GetSwapChain().GetImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = CheckVkResult;
    ImGui_ImplVulkan_Init(&init_info, Vulkan::GetSwapChain().GetRenderPass());
    {
      CommandBufferScoped commandBuffer;
      ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    }
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

  ImGuiInstance::~ImGuiInstance()
  {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiInstance::Begin()
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiInstance::End()
  {
    ImGui::EndFrame();
  }

  void ImGuiInstance::Render(CommandBuffer& commandBuffer)
  {
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
  }

  void ImGuiInstance::CheckVkResult(VkResult err)
  {
    CP_VK_ASSERT(err, "Failed to initialize ImGui");
  }
}