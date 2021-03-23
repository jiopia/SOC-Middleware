#ifndef _CUSTOMIZED_PRIORITY_QUEUE_HPP_
#define _CUSTOMIZED_PRIORITY_QUEUE_HPP_

#include <iostream>
#include <string>
#include <mutex>
#include <queue>
#include <vector>

template <typename T>
class PriorityQueue
{
public:
    PriorityQueue() {}
    ~PriorityQueue() {}

    void Push(const T &node)
    {
        std::lock_guard<std::mutex> lockGuard(m_mtx);
        m_queue.push(node);
    }

    T Top()
    {
        std::lock_guard<std::mutex> lockGuard(m_mtx);
        T t;
        if (!m_queue.empty())
        {
            t = m_queue.top();
        }
        return t;
    }

    void Pop()
    {
        std::lock_guard<std::mutex> lockGuard(m_mtx);
        if (!m_queue.empty())
        {
            m_queue.pop();
        }
    }

    bool Empty()
    {
        std::lock_guard<std::mutex> lockGuard(m_mtx);
        return m_queue.empty();
    }

    size_t Size()
    {
        std::lock_guard<std::mutex> lockGuard(m_mtx);
        return m_queue.size();
    }

private:
    std::mutex m_mtx;
    std::priority_queue<T, std::vector<T>, std::greater<T>> m_queue;
};

#endif //!_CUSTOMIZED_PRIORITY_QUEUE_HPP_