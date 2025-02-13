#include "window_event_manager.hpp"
#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>

namespace lve {
WindowEventManager::WindowEventManager(GLFWwindow *glfwWindow,
                                       WindowEventInterface *interface)
    : glfwWindow(glfwWindow), interface(interface) {}

void WindowEventManager::framebuffferResizedCallback(GLFWwindow *window,
                                                     int width, int height) {
  auto instance =
      static_cast<WindowEventManager *>(glfwGetWindowUserPointer(window));

  instance->interface->onFramebufferResized(width, height);
}

void WindowEventManager::keyCallback(GLFWwindow *window, int key, int scancode,
                                     int action, int mods) {
  auto instance =
      static_cast<WindowEventManager *>(glfwGetWindowUserPointer(window));

  instance->interface->onKeyPress(key, scancode, action, mods);
}

void WindowEventManager::enableFramebufferResizedEvent() {
  glfwSetFramebufferSizeCallback(
      glfwWindow, WindowEventManager::framebuffferResizedCallback);
}

void WindowEventManager::enableKeyEvent() {
  glfwSetKeyCallback(glfwWindow, keyCallback);
}

}; // namespace lve
