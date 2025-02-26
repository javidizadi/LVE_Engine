#include "watchdog.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <functional>
#include <stop_token>
#include <thread>

Watchdog::Watchdog(std::function<void(void *)> callback,
                   std::chrono::duration<double> timeout)
    : _timeout(timeout) {
  _running.store(false);
  _callback = callback;
  _userPtr = nullptr;
  _thread = std::jthread([this](std::stop_token st) { _watchdogLoop(st); });
}

Watchdog::~Watchdog() {
  _thread.request_stop();
  if (_thread.joinable())
    _thread.join();
}

void Watchdog::start() {
  _lastReset = std::chrono::steady_clock::now();
  _running.store(true, std::memory_order_release);
}

void Watchdog::stop() { _running.store(false, std::memory_order_release); }

void Watchdog::reset() { _lastReset = std::chrono::steady_clock::now(); }

void Watchdog::_watchdogLoop(std::stop_token st) {
  while (true) {
    if (st.stop_requested())
      return;

    while (_running.load(std::memory_order_acquire)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));

      if (st.stop_requested())
        return;

      _watchdogTick();
    }
  }
}

void Watchdog::_watchdogTick() {
  auto now = std::chrono::steady_clock::now();
  if (now - _lastReset >= _timeout) {
    _callback(_userPtr);
    stop();
  }
}

void Watchdog::setUserPointer(void *ptr) { _userPtr = ptr; }

void *Watchdog::getUserPointer() { return _userPtr; }
