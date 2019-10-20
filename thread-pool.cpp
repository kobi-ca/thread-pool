#include "thread-pool.hpp"

inline ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(
                [this] { // begin lambda
                    for(;;) {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        while (!this->stop && this->tasks.empty())
                            this->condition.wait(lock);
                        if (this->stop && this->tasks.empty())
                            return;
                        std::function<void()> task(this->tasks.front());
                        this->tasks.pop();
                        lock.unlock();
                        task();
                    }
                } // end lambda
        ); // emplace_back
    }
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }

  condition.notify_all();
  for (auto& w : workers) {
    w.join();
  }
}