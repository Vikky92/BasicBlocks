#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "SafeQueue.h"
class ThreadPool {
 private:
  class ThreadWorker {
   private:
    int mId;
    ThreadPool *mPool;

   public:
    ThreadWorker(ThreadPool *pool, const int id) : mPool(pool), mId(id) {}

    void operator()() {
      std::function<void()> func;
      bool run;
      while (!mPool->mShut) {
        std::unique_lock<std::mutex> tmpLock(mPool->mMutex);
        if (mPool->mQueue.empty()) {
          mPool->mCv.wait(tmpLock);
        }
        run = mPool->mQueue.pop(func);
      }
      if (run) {
        func();
      }
    }
  };

  bool mShut;
  SafeQueue<std::function<void()>> mQueue;
  std::vector<std::thread> mTasks;
  std::mutex mMutex;
  std::condition_variable mCv;

 public:
  ThreadPool(const int n_threads)
      : mTasks(std::vector<std::thread>(n_threads)), mShut(false) {
    for (int i = 0; i < n_threads; i++) {
      mTasks[i] = std::thread(ThreadWorker(this, i));
    }
  }
  ~ThreadPool()
  {
    this->shutdown();
  }
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;

  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  void shutdown() {
    mShut = true;
    mCv.notify_all();

    for (int i = 0; i < mTasks.size(); i++) {
      if (mTasks[i].joinable()) mTasks[i].join();
    }
  }

  // Push a function to be executed asynchronously to the ThreadPool
  template <typename F, typename... Args>
  auto push(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    // Creating a function with bounded parameters
    std::function<decltype(f(args...))()> tmpFunc =
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    // Making a shared ptr to be able to copied / assigned to the threadPool
    auto taskPtr =
        std::make_shared<std::packaged_task<decltype(f(args...))()>>(tmpFunc);

    // Wrapping into a void function
    std::function<void()> wFunc = [taskPtr]() { (*taskPtr)(); };

    // Push to the SafeQueue
    mQueue.push(wFunc);

    // Wake up a waiting thread
    mCv.notify_one();

    // Return future from promise
    return taskPtr->get_future();
  }
};