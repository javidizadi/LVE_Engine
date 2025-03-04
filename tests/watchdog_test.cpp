#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN // Generate main() for doctest
#include "../src/watchdog.hpp"
#include "doctest/doctest.h"
#include <chrono>
#include <thread>

TEST_CASE("Watchdog starts and stops correctly") {
  bool callbackCalled = false;
  Watchdog watchdog([&](void *) { callbackCalled = true; }, nullptr,
                    std::chrono::milliseconds(100));

  SUBCASE("Start and stop without timeout") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Wait less than timeout.
    watchdog.stop();

    CHECK_FALSE(callbackCalled); // Callback should not be called.
  }

  SUBCASE("Start and let timeout occur") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(150)); // Wait longer than timeout.
    watchdog.stop();

    CHECK(callbackCalled); // Callback should be called.
  }
}

TEST_CASE("Watchdog reset works correctly") {
  bool callbackCalled = false;
  Watchdog watchdog([&](void *) { callbackCalled = true; }, nullptr,
                    std::chrono::milliseconds(100));

  SUBCASE("Reset before timeout") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Wait less than timeout.
    watchdog.reset();                   // Reset the timer.
    std::this_thread::sleep_for(
        std::chrono::milliseconds(60)); // Wait again, but total < timeout.
    watchdog.stop();

    CHECK_FALSE(callbackCalled); // Callback should not be called.
  }

  SUBCASE("Reset after timeout") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(150)); // Wait longer than timeout.
    watchdog.reset();                    // Reset after timeout.
    watchdog.stop();

    CHECK(callbackCalled); // Callback should be called.
  }
}

TEST_CASE("Watchdog handles user pointer correctly") {
  int userData = 42;
  int *receivedPointer = nullptr;
  Watchdog watchdog(
      [&](void *ptr) { receivedPointer = static_cast<int *>(ptr); }, &userData,
      std::chrono::milliseconds(100));

  watchdog.start();
  std::this_thread::sleep_for(
      std::chrono::milliseconds(150)); // Wait longer than timeout.
  watchdog.stop();

  CHECK(receivedPointer ==
        &userData); // Callback should receive the correct user pointer.
}

TEST_CASE("Watchdog stops immediately when requested") {
  bool callbackCalled = false;
  Watchdog watchdog([&](void *) { callbackCalled = true; }, nullptr,
                    std::chrono::milliseconds(100));

  SUBCASE("Stop before timeout") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Wait less than timeout.
    watchdog.stop();

    CHECK_FALSE(callbackCalled); // Callback should not be called.
  }

  SUBCASE("Stop after timeout") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(150)); // Wait longer than timeout.
    watchdog.stop();

    CHECK(callbackCalled); // Callback should be called.
  }
}

TEST_CASE("Watchdog handles multiple starts and stops") {
  bool callbackCalled = false;
  Watchdog watchdog([&](void *) { callbackCalled = true; }, nullptr,
                    std::chrono::milliseconds(100));

  SUBCASE("Start, stop, and start again") {
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)); // Wait less than timeout.
    watchdog.stop();
    CHECK_FALSE(callbackCalled); // Callback should not be called.

    callbackCalled = false; // Reset the flag.
    watchdog.start();
    std::this_thread::sleep_for(
        std::chrono::milliseconds(150)); // Wait longer than timeout.
    watchdog.stop();
    CHECK(callbackCalled); // Callback should be called.
  }
}

TEST_CASE("Watchdog handles rapid resets") {
  bool callbackCalled = false;
  Watchdog watchdog([&](void *) { callbackCalled = true; }, nullptr,
                    std::chrono::milliseconds(100));

  SUBCASE("Rapid resets prevent timeout") {
    watchdog.start();
    for (int i = 0; i < 10; ++i) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(50)); // Wait less than timeout.
      watchdog.reset();                   // Reset the timer repeatedly.
    }
    watchdog.stop();

    CHECK_FALSE(callbackCalled); // Callback should not be called.
  }
}
