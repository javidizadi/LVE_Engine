#include "window_event_manager.hpp"
#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <memory>

namespace lve {
WindowEventManager::WindowEventManager(GLFWwindow *glfwWindow,
                                       WindowEventInterface *interface)
    : glfwWindow(glfwWindow), interface(interface) {}

WindowEventManager *
WindowEventManager::getWindowManagerInstance(GLFWwindow *window) {
  void *eventManagerPtr = glfwGetWindowUserPointer(window);
  return (WindowEventManager *)eventManagerPtr;
}

void WindowEventManager::framebuffferResizedCallback(GLFWwindow *window,
                                                     int width, int height) {
  auto instance = getWindowManagerInstance(window);
  if (instance->onFramebufferResizeDoneEnabled)
    instance->_resizeWatchdogPtr->start();
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
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffferResizedCallback);
    break;

  case lve::WindowEvents::onFramebufferResizeDone:
    _resizeWatchdogPtr = std::make_unique<Watchdog>(
        resizeWatchdogCallback, this, std::chrono::milliseconds(100));
    onFramebufferResizeDoneEnabled = true;
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

  case lve::WindowEvents::onFramebufferResizeDone:
    onFramebufferResizeDoneEnabled = false;
    _resizeWatchdogPtr.reset();
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
