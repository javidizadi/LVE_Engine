#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

namespace lve {

Window::Window(int width, int height, std::string window_name)
    : windowName(window_name), WIDTH(width), HEIGHT(height) {
  initWindow();
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window =
      glfwCreateWindow(WIDTH, HEIGHT, windowName.c_str(), nullptr, nullptr);
}

Window::~Window() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::createWindowSurface(const VkInstance &instance,
                                 VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
    throw std::runtime_error("Failed to create Window Surface.");
}

VkExtent2D Window::getExtent() {
  return {
      .width = static_cast<uint32_t>(WIDTH),
      .height = static_cast<uint32_t>(HEIGHT),
  };
}

} // namespace lve
