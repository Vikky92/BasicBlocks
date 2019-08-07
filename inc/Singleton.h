#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

const int tenMill = 10000000;

class Singleton {
 public:
  static Singleton* getInstance() {
    Singleton* ins = instance.load();
    {
      if (!ins) {
        mMutex.lock();
        ins = instance.load();

        if (!ins) {
          ins = new Singleton();
          instance.store(ins);
        }
      }
    }

    return ins;
  }

 private:
  Singleton() = default;
  ~Singleton() = default;
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

  static std::atomic<Singleton*> instance;
  static std::mutex mMutex;
};