#pragma once

#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

#include <memory>
#include <vector>

#include <vulkan/vulkan_core.h>

class FirstApp {
private:
  static constexpr int HEIGHT = 500;
  static constexpr int WIDTH = 500;

  lve::Window window;
  lve::Device device;
  lve::SwapChain swapChain;
  std::unique_ptr<lve::Pipeline> pipeline;
  std::unique_ptr<lve::Model> model;

  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;

  void loadModels();
  void createSwapChain();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

public:
  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();
};
