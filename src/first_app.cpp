#include <iostream>

#include "first_app.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

FirstApp::FirstApp()
    : window(WIDTH, HEIGHT, "VULKAN FUKIN TRIANGLE"), device(window),
      swapChain(device, window.getExtent()) {
  loadModels();
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

FirstApp::~FirstApp() {
  vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
  while (!window.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(device.device());
}

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
      device, "./shaders/simple_triangle.vert.spv",
      "./shaders/simple_triangle.frag.spv", pipelineConfig);
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

void createNextSierpinski(std::vector<lve::Model::Vertex> &model, size_t n) {

  model.reserve(std::pow(3, n + 1));

  for (size_t j = 0; j < n; j++) {

    auto triangleCount = model.size() / 3;

    auto A = model.begin();
    auto B = A + 1;
    auto C = A + 2;

    for (size_t i = 0; i < triangleCount; i++) {
      auto midAB = (*A + *B) / 2.0f;
      auto midAC = (*A + *C) / 2.0f;
      auto midBC = (*B + *C) / 2.0f;

      auto pos = model.insert(A + 1, {midAB, midAC, midAB});
      B = pos + 3;
      pos = model.insert(B + 1, {midBC, midAC, midBC});

      A = pos + 4;
      B = A + 1;
      C = A + 2;
    }

    std::cout << "N: " << j + 1 << std::endl;
  }
}

void FirstApp::loadModels() {
  std::vector<lve::Model::Vertex> vertices{
      {{-1.0, 1.0}},
      {{0, -1.0}},
      {{1, 1}},
  };

  createNextSierpinski(vertices, 7);

  model = std::make_unique<lve::Model>(device, vertices);
}
