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
    InitializeImGui();
    InitializeDescriptorSetLayout();
  }

  ImGuiInstance::~ImGuiInstance()
  {
    vkDestroyDescriptorSetLayout(Vulkan::GetDevice(), descriptorSetLayout, nullptr);
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

  std::unique_ptr<DescriptorSet> ImGuiInstance::CreateDescriptorSet()
  {
    return std::make_unique<DescriptorSet>(*descriptorPool, descriptorSetLayout, shaderBindings);
  }

  void ImGuiInstance::InitializeImGui()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(Vulkan::GetWindow().GetWindow(), true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = Vulkan::GetInstance();
    initInfo.PhysicalDevice = Vulkan::GetDevice().GetPhysicalDevice();
    initInfo.Device = Vulkan::GetDevice();
    initInfo.QueueFamily = Vulkan::GetDevice().GetGraphicsQueueFamily();
    initInfo.Queue = Vulkan::GetDevice().GetGraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = *descriptorPool.get();
    initInfo.Subpass = 0;
    initInfo.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
    initInfo.ImageCount = Vulkan::GetSwapChain().GetImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = CheckVkResult;
    ImGui_ImplVulkan_Init(&initInfo, Vulkan::GetSwapChain().GetRenderPass());
    {
      CommandBufferScoped commandBuffer;
      ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    }
    ImGui_ImplVulkan_DestroyFontUploadObjects();
  }

  void ImGuiInstance::InitializeDescriptorSetLayout()
  {
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings{1};
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = layoutBindings.data();
    CP_VK_ASSERT(vkCreateDescriptorSetLayout(Vulkan::GetDevice(), &createInfo, nullptr, &descriptorSetLayout), "Failed to create ImGui DescriptorSetLayout");

    ShaderBinding binding;
    binding.name = "texture";
    binding.set = 0;
    binding.binding = 0;
    binding.arraySize = 1;
    binding.bindingType = BindingType::Sampler2D;
    binding.shaderType = ShaderType::Fragment;

    shaderBindings = {binding};
  }

  void ImGuiInstance::CheckVkResult(VkResult err)
  {
    CP_VK_ASSERT(err, "Failed to initialize ImGui");
  }
}