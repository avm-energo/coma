#pragma once

#include <unordered_set>

#include <QDebug>
#include <functional>
#include <queue>

namespace Utils
{

template <typename T> //
class UniqueQueue final
{
private:
    std::queue<T> m_data;
    std::unordered_set<std::reference_wrapper<T>> m_unique;

public:
    typedef typename std::queue<T>::value_type value_type;
    typedef typename std::queue<T>::size_type size_type;
    typedef typename std::queue<T>::reference reference;
    typedef typename std::queue<T>::const_reference const_reference;

    explicit UniqueQueue() noexcept = default;

    explicit UniqueQueue(const UniqueQueue<T> &other) noexcept
    {
        // m_unique holds references into m_data's own storage, so a member-wise copy of
        // other.m_unique would leave references pointing at other's elements instead of ours.
        // Rebuild the index against the freshly copied elements instead.
        std::queue<T> tmp(other.m_data);
        while (!tmp.empty())
        {
            m_data.push(std::move(tmp.front()));
            m_unique.insert(std::ref(m_data.back()));
            tmp.pop();
        }
    }

    explicit UniqueQueue(UniqueQueue<T> &&other) noexcept
        : m_data(std::move(other.m_data))
        , m_unique(std::move(other.m_unique))
    {
    }

    UniqueQueue<T> &operator=(const UniqueQueue<T> &other) noexcept
    {
        if (this != &other)
        {
            UniqueQueue<T> tmp(other);
            swap(tmp);
        }
        return *this;
    }

    UniqueQueue<T> &operator=(UniqueQueue<T> &&other) noexcept
    {
        m_data = std::move(other.m_data);
        m_unique = std::move(other.m_unique);
        return *this;
    }

    inline reference front()
    {
        return m_data.front();
    }

    inline const_reference front() const
    {
        return m_data.front();
    }

    inline reference back()
    {
        return m_data.back();
    }

    inline const_reference back() const
    {
        return m_data.back();
    }

    inline bool empty() const noexcept
    {
        return m_data.empty();
    }

    inline size_type size() const noexcept
    {
        return m_data.size();
    }

    void push(const value_type &value)
    {
        if (m_unique.find(value) == m_unique.end())
        {
            m_data.push(value);
            m_unique.insert(std::ref(m_data.back()));
        }
    }

    void push(value_type &&value)
    {
        if (m_unique.find(value) == m_unique.end())
        {
            m_data.push(std::move(value));
            m_unique.insert(std::ref(m_data.back()));
        }
    }

    void pop()
    {
        [[maybe_unused]] auto result = m_unique.erase(std::ref(m_data.front()));
        if (!result)
            qDebug() << "Value is not found in the uniQueue (data size:" << m_data.size()
                      << ", index size:" << m_unique.size() << ") - the two are now out of sync,"
                      << "the stale index entry will silently block future pushes of equal content";
        m_data.pop();
    }

    void swap(UniqueQueue<T> &other)
    {
        std::swap(m_data, other.m_data);
        std::swap(m_unique, other.m_unique);
    }
};

} // namespace Utils
