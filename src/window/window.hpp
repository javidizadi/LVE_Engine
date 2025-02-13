#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window_event_interface.hpp"
#include "window_event_manager.hpp"
#include <memory>
#include <string>
#include <vulkan/vulkan_core.h>

namespace lve {

class Window {
private:
  void initWindow();
  void initEventManager(WindowEventInterface *eventInterface);

  std::string windowName;
  GLFWwindow *window;
  int width;
  int height;

public:
  WindowEventManager *eventManager;

  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  Window(int width, int height, std::string name,
         WindowEventInterface *eventInterface);
  ~Window();
  bool shouldClose();
  void createWindowSurface(const VkInstance &instance, VkSurfaceKHR *surface);
  VkExtent2D getExtent();
};

} // namespace lve
