#pragma once

#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>

namespace lve {

enum WindowEvents {
  onKeyPress,
  onFramebufferResized,
  onWindowResized,
};

class WindowEventManager {

private:
  WindowEventInterface *interface;
  GLFWwindow *glfwWindow;

  static WindowEventManager *getWindowManagerInstance(void *p);
  static void framebuffferResizedCallback(GLFWwindow *window, int width,
                                          int height);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void windowResizedCallback(GLFWwindow *window, int width, int height);

public:
  void enableEvent(WindowEvents event);
  void disableEvent(WindowEvents event);

  WindowEventManager(GLFWwindow *glfwWindow, WindowEventInterface *interface);
};

} // namespace lve
