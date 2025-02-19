
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

class Watchdog {

private:
  std::function<void()> _callback;
  std::atomic_bool _running;
  const std::chrono::duration<double> _timeout;
  std::chrono::steady_clock::time_point _lastReset;
  std::mutex _mutex;
  std::thread _thread;
  std::condition_variable _cv;

  void _watchdogLoop();

public:
  Watchdog(std::function<void()> callback,
           std::chrono::duration<double> duration);
  ~Watchdog();
  void start();
  void stop();
  void reset();
};
