#include "device.hpp"
#include "pipeline.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <thread>

int main() {

  lve::Window window(1000, 1000, "HELLO VULKAN!");

  lve::Device device(window);

  lve::Pipeline pipeline(
      device, "./shaders/simple_triangle.vert.spv",
      "./shaders/simple_triangle.frag.spv",
      lve::Pipeline::createDefaultPipelineConfig(1000, 1000));

  while (!window.shouldClose()) {
    std::this_thread::yield();
    glfwPollEvents();
  }

  return 0;
}
