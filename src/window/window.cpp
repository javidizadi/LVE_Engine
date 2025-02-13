#include "window.hpp"

#include "window_event_interface.hpp"
#include "window_event_manager.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

namespace lve {

Window::Window(int width, int height, std::string name,
               WindowEventInterface *eventInterface)
    : windowName(name), width(width), height(height) {
  initWindow();
  initEventManager(eventInterface);
}

void Window::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window =
      glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void Window::initEventManager(WindowEventInterface *eventInterface) {
  eventManager = new WindowEventManager(window, eventInterface);
  glfwSetWindowUserPointer(window, eventManager);
}

Window::~Window() {
  delete eventManager;
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
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
  };
}

} // namespace lve
