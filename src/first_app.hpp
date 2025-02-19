#pragma once

#include "device.hpp"
#include "model.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "watchdog.hpp"
#include "window/window.hpp"

#include <memory>
#include <vector>

#include <vulkan/vulkan_core.h>

class FirstApp : private lve::WindowEventInterface {
private:
  int height = 800;
  int width = 800;

  bool resizeInProgress;
  Watchdog resizeWatchdog;

  lve::Window window;
  lve::Device device;
  lve::SwapChain swapChain;
  std::unique_ptr<lve::Pipeline> pipeline;
  std::unique_ptr<lve::Model> model;

  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;

  void waitForResize();
  static void resizeWatchdogCallback(void *);

  void loadModels();
  void createSwapChain();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();

  void onFramebufferResized(int width, int height) override;

public:
  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();
};
