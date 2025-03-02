#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

class Watchdog {

private:
  std::function<void(void *)> _callback;
  std::atomic_bool _running;
  const std::chrono::duration<double> _timeout;
  std::chrono::steady_clock::time_point _lastReset;
  std::jthread _thread;
  std::mutex _mutex;
  std::condition_variable_any _stopCv;

  void *_userPtr;

  void _watchdogLoop(std::stop_token st);
  void _watchdogTick();

public:
  Watchdog(std::function<void(void *)> callback,
           std::chrono::duration<double> duration);
  ~Watchdog();
  void start();
  void stop();
  void reset();

  void setUserPointer(void *);
  void *getUserPointer();
};
