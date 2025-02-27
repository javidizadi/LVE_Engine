#include "window_event_manager.hpp"
#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>
#include <chrono>

namespace lve {
WindowEventManager::WindowEventManager(GLFWwindow *glfwWindow,
                                       WindowEventInterface *interface)
    : glfwWindow(glfwWindow), interface(interface),
      resizeWatchdog(resizeWatchdogCallback, std::chrono::seconds(1)) {
  resizeWatchdog.setUserPointer(interface);
}

constexpr WindowEventManager *
WindowEventManager::getWindowManagerInstance(GLFWwindow *window) {
  void *eventManagerPtr = glfwGetWindowUserPointer(window);
  return (WindowEventManager *)eventManagerPtr;
}

void WindowEventManager::framebuffferResizedCallback(GLFWwindow *window,
                                                     int width, int height) {
  auto instance = getWindowManagerInstance(window);
  instance->resizeWatchdog.start();
  instance->interface->onFramebufferResized(width, height);
}

void WindowEventManager::keyCallback(GLFWwindow *window, int key, int scancode,
                                     int action, int mods) {
  auto instance = getWindowManagerInstance(window);
  instance->interface->onKeyPress(key, scancode, action, mods);
}

void WindowEventManager::windowResizedCallback(GLFWwindow *window, int width,
                                               int height) {
  auto instance = getWindowManagerInstance(window);
  instance->interface->onWindowResized(width, height);
}

void WindowEventManager::mouseButtonCallback(GLFWwindow *window, int button,
                                             int action, int mods) {
  auto instance = getWindowManagerInstance(window);
  instance->interface->onMouseButton(button, action, mods);
}

void WindowEventManager::windowRefreshCallback(GLFWwindow *window) {
  auto instance = getWindowManagerInstance(window);
  instance->interface->onWindowRefresh();
}

void WindowEventManager::resizeWatchdogCallback(void *userPtr) {
  auto instance = (WindowEventManager *)userPtr;
  instance->interface->onFramebufferResizingDone();
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
    resizeWatchdog.setUserPointer(this);
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffferResizedCallback);
    break;

  case WindowEvents::onMouseButton:
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    break;

  case WindowEvents::onWindowRefresh:
    glfwSetWindowRefreshCallback(glfwWindow, windowRefreshCallback);
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

  case WindowEvents::onMouseButton:
    glfwSetMouseButtonCallback(glfwWindow, nullptr);
    break;

  case WindowEvents::onWindowRefresh:
    glfwSetWindowRefreshCallback(glfwWindow, nullptr);
    break;
  }
}

}; // namespace lve
