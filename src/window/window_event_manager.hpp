#pragma once

#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>

namespace lve {

class WindowEventManager {

private:
  WindowEventInterface *interface;
  GLFWwindow *glfwWindow;
  static void framebuffferResizedCallback(GLFWwindow *window, int width,
                                          int height);

  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

public:
  void enableFramebufferResizedEvent();
  void enableKeyEvent();

  WindowEventManager(GLFWwindow *glfwWindow, WindowEventInterface *interface);
};

} // namespace lve
