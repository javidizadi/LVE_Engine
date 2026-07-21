#include "first_app.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window/window.hpp"
#include "window/window_event_manager.hpp"
#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

FirstApp::FirstApp()
    : window(width, height, "VULKAN FUKIN TRIANGLE", this), device(window),
      swapChain(device, window.getExtent()),
      resizeWatchdog(resizeWatchdogCallback, this,
                     std::chrono::milliseconds(1000)),
      resizeInProgress(false) {
  loadModels();
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {

  window.eventManager->enableEvent(lve::WindowEvents::onFramebufferResized);

  while (!window.shouldClose()) {
    glfwPollEvents();
    waitForResize();
    drawFrame();
  }

  vkDeviceWaitIdle(device.device());
}

void FirstApp::onFramebufferResized(int width, int height) {
  this->width = width;
  this->height = height;

  resizeWatchdog.start();
  resizeWatchdog.reset();
  resizeInProgress = true;
}

void FirstApp::waitForResize() {
  if (!resizeInProgress)
    return;

  while (resizeInProgress)
    glfwPollEvents();

  resizeWatchdog.stop();
}

void FirstApp::resizeWatchdogCallback(void *p) {
  auto instance = (FirstApp *)(p);
  instance->resizeInProgress = false;
};

void FirstApp::createPipelineLayout() {

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
  };

  if (vkCreatePipelineLayout(device.device(), &pipelineLayoutCreateInfo,
                             nullptr, &pipelineLayout) != VK_SUCCESS)
    throw std::runtime_error("Failed to create pipeline layout.");
}

void FirstApp::createPipeline() {
  auto pipelineConfig = lve::Pipeline::createDefaultPipelineConfig(
      swapChain.width(), swapChain.height());
  pipelineConfig.renderPass = swapChain.getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;

  pipeline = std::make_unique<lve::Pipeline>(
      device, "shaders/simple_triangle.vert.spv",
      "shaders/simple_triangle.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(swapChain.imageCount());

  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = device.getCommandPool(),
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
  };

  if (vkAllocateCommandBuffers(device.device(), &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS)
    throw std::runtime_error("Failed to allocate command buffers.");

  for (uint8_t i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
      throw std::runtime_error("Failed to begin recording command buffer.");

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = swapChain.getRenderPass(),
        .framebuffer = swapChain.getFrameBuffer(i),
        .renderArea =
            VkRect2D{
                .offset = VkOffset2D{.x = 0, .y = 0},
                .extent = swapChain.getSwapChainExtent(),
            },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    pipeline->bind(commandBuffers[i]);
    // vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
    model->bind(commandBuffers[i]);
    model->draw(commandBuffers[i]);

    vkCmdEndRenderPass(commandBuffers[i]);

    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
      throw std::runtime_error("Failed to end command buffer.");
  };
}

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  auto result = swapChain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw std::runtime_error("Failed to acquire swap chain image.");

  result =
      swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to preset swap chain image.");
  }
}

void FirstApp::loadModels() {
  std::vector<lve::Model::Vertex> vertices{
      {{-0.5, 0}, {1.0, 0.4, 0.3}},
      {{0.5, 0}, {0.4, 0.5, 0.2}},
      {{0, -0.5}, {0.4, 0.6, 0.9}},
  };

  model = std::make_unique<lve::Model>(device, vertices);
}
