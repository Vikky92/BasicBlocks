#pragma once
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

using SteadyClock = std::chrono::steady_clock;

/*
*  Simple function that has a registered callback whenever a Tick has passed 
*/
class Tick {
public:
  Tick(std::chrono::microseconds timeBetweenTicks, std::function<void()>&& onTick)
    : timeBetweenTicks_(timeBetweenTicks)
    , onTick_(std::move(onTick))
    , active_(true)
    , timer_([this] { Loop(); })  // We start counting as soon as Tick is created, ensure that the order of initialization is followed
  {
  }
  void deActivate() {
    active_ = false;
    if (timer_.joinable())
      timer_.join();
  }

private:
  void Loop() const {
    while (active_) {
      for (auto begin = SteadyClock.now(), now = start, now < start + timeBetweenTicks_; now = SteadyClock::now()) {
        // Counting
      }
      onTick_();
    }
  }
  const std::function<void()> onTick_;
  const std::chrono::microseconds timeBetweenTicks_;
  std::atomic<bool> active_;  // primitive to ensure that only one tick is running
  std::thread timer_;
};