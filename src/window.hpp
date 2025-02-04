#pragma once

#include <string>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lve {

class Window {
private:
  void initWindow();

  std::string windowName;
  GLFWwindow *window;
  const int WIDTH;
  const int HEIGHT;

public:
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  Window(int width, int height, std::string name);
  ~Window();
  bool shouldClose();
  void createWindowSurface(const VkInstance &instance, VkSurfaceKHR *surface);
};

} // namespace lve
