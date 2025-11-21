#pragma once

#include <atomic>
#include <cstddef>

#include <QVector>

template <typename T>
class RingBuffer
{
public:
    explicit RingBuffer(size_t capacity) :
        m_capacity(capacity + 1), m_buf(m_capacity), m_head(0), m_tail(0)
    {}

    bool
    push(const T &item)
    {
        size_t next = (m_head.load(std::memory_order_relaxed) + 1) % m_capacity;

        if(next == m_tail.load(std::memory_order_acquire)) return false;

        m_buf[m_head.load(std::memory_order_relaxed)] = item;

        m_head.store(next, std::memory_order_release);

        return true;
    }

    size_t
    popBulk(QVector<T> &out, size_t maxCount)
    {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        size_t head = m_head.load(std::memory_order_acquire);

        if(tail == head) return 0;

        size_t count = (head + m_capacity - tail) % m_capacity;

        if(count > maxCount) count = maxCount;

        out.reserve(out.size() + count);

        for(size_t i = 0; i < count; ++i)
        {
            out.push_back(m_buf[tail]);
            tail = (tail + 1) % m_capacity;
        }

        m_tail.store(tail, std::memory_order_release);

        return count;
    }

    void
    clear()
    {
        m_head.store(0);
        m_tail.store(0);
    }

    std::size_t
    capacity() const
    {
        return m_capacity - 1;
    }

private:
    const std::size_t   m_capacity;
    std::vector<T>      m_buf;
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
};

