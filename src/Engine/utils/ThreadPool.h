#pragma once

#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>
#include "Vector.h"

struct TaskQueue {
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::atomic<int> m_tasksLeft = 0;


    template<typename TCallback>
    void addTask(TCallback &&callback) {
        std::lock_guard guard(m_mutex);
        m_tasks.push(std::forward<TCallback>(callback));
        m_tasksLeft++;
    }

    void getTask(std::function<void()> &callback) {
        std::lock_guard guard(m_mutex);
        if (m_tasks.empty()) {
            return;
        }
        callback = std::move(m_tasks.front());
        m_tasks.pop();
    }

    static void wait() {
        std::this_thread::yield();
    }

    void waitForCompletion() {
        while (m_tasksLeft > 0) {
            wait();
        }
    }

    void taskDone() {
        m_tasksLeft--;
    }
};

struct Worker {
    std::thread m_thread;
    TaskQueue *m_queue = nullptr;
    bool m_isRunning = true;
    std::function<void()> m_task = nullptr;

    explicit Worker(TaskQueue &queue) : m_queue(&queue) {
        m_thread = std::thread([this]() {
            run();
        });
    }

    void run() {
        while (m_isRunning) {
            m_queue->getTask(m_task);
            if (m_task == nullptr) {
                TaskQueue::wait();
            } else {
                m_task();
                m_queue->taskDone();
                m_task = nullptr;
            }
        }
    }

    void stop() {
        m_isRunning = false;
        m_thread.join();
    }
};

struct ThreadPool {
    TaskQueue m_queue;
    std::vector<Worker> m_workers;
    int m_threadsNum;

    explicit ThreadPool(int threadsNum) : m_threadsNum(threadsNum) {
        m_workers.reserve(threadsNum);
        for (int i = 0; i < threadsNum; i++) {
            m_workers.emplace_back(m_queue);
        }
    }

    ~ThreadPool() {
        for (Worker &worker: m_workers) {
            worker.stop();
        }
    }

    template<typename TCallback>
    void addTask(TCallback &&callback) {
        m_queue.addTask(std::forward<TCallback>(callback));
    }

    void waitForCompletion() {
        m_queue.waitForCompletion();
    };

    template<typename TCallback>
    void dispatch(int maxCount, TCallback &&callback) {
        const int batchSize = maxCount / m_threadsNum;

        for (int i = 0; i < m_threadsNum; i++) {
            const int start = batchSize * i;
            const int end = start + batchSize;

            addTask([&callback, start, end]() {
                callback(start, end);
            });
        }

        if (batchSize * m_threadsNum < maxCount) {
            const int start = batchSize * m_threadsNum;
            const int end = maxCount;
            callback(start, end);
//            addTask([&m_callback, start, end]() {
//                m_callback(start, end);
//            });
        }

        waitForCompletion();
    }
};