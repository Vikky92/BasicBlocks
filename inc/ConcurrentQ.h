#include <condition_variable>
#include <mutex>
#include <queue>

using namespace std;

template <typename Data>
class CQueue {
 private:
  std::queue<Data> mQueue;
  static std::mutex mMutex;
  static condition_variable mCv;

 public:
  void push(Data const& data) {
    mMutex.lock();
    mQueue.push(data);
    mMutex.unlock();

    mCv.notify_one();
  }

  bool empty() const {
    mMutex.lock();
    return mQueue.empty();
  }

  bool try_pop(Data& val) {
    mMutex.lock();
    if (mQueue.empty()) {
      return false;
    }

    val = mQueue.front();
    mQueue.pop();
    return true;
  }

  void wait_and_pop(Data& val) {
    mMutex.lock();
    while (mQueue.empty()) {
      unique_lock<std::mutex> lock(mMutex);
      mCv.wait(lock);
    }

    val = mQueue.front();
    mQueue.pop();
  }
};
