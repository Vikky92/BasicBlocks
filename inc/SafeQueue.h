#pragma once
#include <mutex>
#include <queue>

// Thread-safe implementatation of std::queue
template <typename T>

class SafeQueue
{
private:
  std::queue<T> mQueue;
  std::mutex mMutex;

public:
  SafeQueue() {}

  SafeQueue(SafeQueue &cQueue)
  {
    // TODO:
  }

  ~SafeQueue() {}

  bool empty()
  {
    std::unique_lock<std::mutex> tmpLock(mMutex);
    return mQueue.empty();
  }

  int size()
  {
    std::unique_lock<std::mutex> tmpLock(mMutex);
    return mQueue.size();
  }

  void push(T &t)
  {
    std::unique_lock<std::mutex> tmpLock(mMutex);
    mQueue.push(t);
  }
  bool pop(T &t)
  {
    if (mQueue.empty())
    {
      return false;
    }
    t = std::move(mQueue.front());
    mQueue.pop();
    return true;
  }
};