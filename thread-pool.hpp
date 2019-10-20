//
// Created by kobi on 10/20/19.
//

#ifndef THREAD_POOL_THREAD_POOL_HPP
#define THREAD_POOL_THREAD_POOL_HPP

#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <memory>

class ThreadPool {
public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
    typedef typename std::result_of<F(Args...)>::type return_type;

    // don't allow enqueueing after stopping the pool
    if (stop)
        throw std::runtime_error("enqueue on stopped ThreadPool");

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

#endif //THREAD_POOL_THREAD_POOL_HPP
