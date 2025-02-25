#include "watchdog.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

Watchdog::Watchdog(std::function<void(void *)> callback,
                   std::chrono::duration<double> timeout)
    : _running(false), _callback(callback), _timeout(timeout),
      _userPtr(nullptr) {}

Watchdog::~Watchdog() {
  if (_thread.joinable())
    _thread.join();
}

void Watchdog::start() {
  bool expected = false;
  if (_running.compare_exchange_weak(expected, true,
                                     std::memory_order_relaxed)) {
    _thread = std::thread(&Watchdog::_watchdogLoop, this);
  }
}

void Watchdog::stop() {
  _running.store(false, std::memory_order_release);
  _cv.notify_all();

  if (_thread.joinable())
    _thread.join();
}

void Watchdog::reset() {
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _lastReset = std::chrono::steady_clock::now();
  }
  _cv.notify_all();
}

void Watchdog::_watchdogLoop() {
  std::unique_lock<std::mutex> lock(_mutex);
  while (_running.load(std::memory_order_acquire)) {

    auto now = std::chrono::steady_clock::now();

    if (now - _lastReset > _timeout) {
      if (_callback)
        _callback(_userPtr);
      break;
    }

    _cv.wait_for(lock, _timeout);
  }

  _running.store(false, std::memory_order_relaxed);
}

void Watchdog::setUesrPointer(void *ptr) { _userPtr = ptr; }

void *Watchdog::getUserPointer() { return _userPtr; }
