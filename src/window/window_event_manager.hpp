#pragma once

#include "../watchdog.hpp"
#include "window_event_interface.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace lve {

enum WindowEvents {
  onKeyPress,
  onFramebufferResized,
  onFramebufferResizeDone,
  onWindowResized,
  onMouseButton,
  onWindowRefresh,
};

class WindowEventManager {

private:
  WindowEventInterface *interface;
  GLFWwindow *glfwWindow;

  bool onFramebufferResizeDoneEnabled;
  std::unique_ptr<Watchdog> _resizeWatchdogPtr;

  static void resizeWatchdogCallback(void *);
  static WindowEventManager *getWindowManagerInstance(GLFWwindow *window);

  static void framebuffferResizedCallback(GLFWwindow *window, int width,
                                          int height);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);
  static void windowResizedCallback(GLFWwindow *window, int width, int height);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action,
                                  int mods);
  static void windowRefreshCallback(GLFWwindow *window);

public:
  void enableEvent(WindowEvents event);
  void disableEvent(WindowEvents event);

  WindowEventManager(GLFWwindow *glfwWindow, WindowEventInterface *interface);
};

} // namespace lve
