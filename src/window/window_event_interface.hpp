#pragma once

namespace lve {
class WindowEventInterface {
public:
  virtual void onKeyPress(int key, int scancode, int action, int mods) {};
  virtual void onFramebufferResized(int width, int height) {};
  virtual void onWindowResized(int width, int height) {};
  virtual void onMouseButton(int button, int action, int mods) {};
  virtual void onWindowRefresh() {};
  virtual void onFramebufferResizingDone() {};
};

} // namespace lve
