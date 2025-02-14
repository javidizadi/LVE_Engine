#include "window_event_manager.hpp"
#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>

namespace lve {
WindowEventManager::WindowEventManager(GLFWwindow *glfwWindow,
                                       WindowEventInterface *interface)
    : glfwWindow(glfwWindow), interface(interface) {}

WindowEventManager *WindowEventManager::getWindowManagerInstance(void *p) {
  return static_cast<WindowEventManager *>(p);
}

void WindowEventManager::framebuffferResizedCallback(GLFWwindow *window,
                                                     int width, int height) {
  auto instance = getWindowManagerInstance(glfwGetWindowUserPointer(window));
  instance->interface->onFramebufferResized(width, height);
}

void WindowEventManager::keyCallback(GLFWwindow *window, int key, int scancode,
                                     int action, int mods) {
  auto instance = getWindowManagerInstance(glfwGetWindowUserPointer(window));
  instance->interface->onKeyPress(key, scancode, action, mods);
}

void WindowEventManager::windowResizedCallback(GLFWwindow *window, int width,
                                               int height) {
  auto instance = getWindowManagerInstance(glfwGetWindowUserPointer(window));
  instance->interface->onWindowResized(width, height);
}

void WindowEventManager::enableEvent(WindowEvents event) {
  switch (event) {
  case WindowEvents::onWindowResized:
    glfwSetWindowSizeCallback(glfwWindow, windowResizedCallback);
    break;

  case WindowEvents::onKeyPress:
    glfwSetKeyCallback(glfwWindow, keyCallback);
    break;

  case WindowEvents::onFramebufferResized:
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffferResizedCallback);
    break;
  }
}

void WindowEventManager::disableEvent(WindowEvents event) {
  switch (event) {
  case WindowEvents::onWindowResized:
    glfwSetWindowSizeCallback(glfwWindow, nullptr);
    break;

  case WindowEvents::onKeyPress:
    glfwSetKeyCallback(glfwWindow, nullptr);
    break;

  case WindowEvents::onFramebufferResized:
    glfwSetFramebufferSizeCallback(glfwWindow, nullptr);
    break;
  }
}

}; // namespace lve
