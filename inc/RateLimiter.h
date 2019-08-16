#pragma once

#include <chrono>
#include <mutex>

struct HttpResult {
  enum class Code { Cool = 200,
                    NotCool = 429 };

  HttpResult(Code code)
    : code(code) {}

  const Code code;
};

class RateLimiter {
public:
  RateLimiter(int maxRequests, int timeFrames)
    : hitQueue_(timeFrames)
    , tick_(std::chrono::seconds(1) / timeFrames, [this] { OnTimeFrame(); })
    , maxRPS_(maxRequests) {
  }
  ~RateLimiter() {
    tick_.deActivate();
  }

private:
  std::mutex mutex_;
  HitQueue hitQueue_;
  Tick tick_;
  const int maxRPS_;
};