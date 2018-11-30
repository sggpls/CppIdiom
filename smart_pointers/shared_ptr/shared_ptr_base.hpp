#pragma once
#ifndef SHARED_PTR_BASE_H
#define SHARED_PTR_BASE_H

#include <atomic>
#include <type_traits>
#include <utility>
#include <cassert>

namespace lvn
{

class sp_counter_base 
{
public:
    sp_counter_base(const sp_counter_base&) = delete;
    sp_counter_base& operator=(const sp_counter_base&) = delete;

    sp_counter_base()
        : m_use_count(1)
        , m_weak_count(1)
    {}

    virtual ~sp_counter_base() = default;
    virtual void dispose() = 0; // releases the resources managed by *this
    virtual void destroy() { delete this; }

    void add_ref_copy() { std::atomic_fetch_add(&m_use_count, 1); }

    void release()
    {
        if (std::atomic_fetch_add(&m_use_count, -1) == 1) {
            dispose();

            // todo: add memory barrier

            if (std::atomic_fetch_add(&m_weak_count, -1) == 1) {
                destroy();
            }
        }
    }

    void weak_add_ref() { std::atomic_fetch_add(&m_weak_count, 1); }

    void weak_release()
    {
        if (std::atomic_fetch_add(&m_weak_count, -1) == 1) {
            destroy();
        }
    }

    int get_use_count() const { return m_use_count; }

private:
    std::atomic<int> m_use_count;   // shared
    std::atomic<int> m_weak_count;  // shared + weak
};

template <typename Ptr>
class sp_counter_ptr : public sp_counter_base
{
public:
    explicit sp_counter_ptr(Ptr ptr)
        : m_ptr(ptr)
    {}

    void dispose() override 
    {
        delete m_ptr;        
    }

    void destroy() override {
        delete this; // suicide
    }

    sp_counter_ptr(const sp_counter_ptr&) = delete;
    sp_counter_ptr& operator=(const sp_counter_ptr&) = delete;

protected:
    Ptr m_ptr;
};

class shared_count
{
public:
    constexpr shared_count()
        : m_ptr(nullptr)
    {}

    template <typename Ptr>
    explicit shared_count(Ptr ptr)
        : m_ptr(nullptr)
    {
        try {
            m_ptr = new sp_counter_ptr<Ptr>(ptr);
        } catch (...) {
            delete ptr;
            throw;
        }
    }

    ~shared_count()
    {
        if (m_ptr)
            m_ptr->release();
    }

    shared_count(const shared_count& r)
        : m_ptr(r.m_ptr)
    {
        if (m_ptr)
            m_ptr->add_ref_copy();
    }

    shared_count& operator=(const shared_count& r)
    {
        sp_counter_base* tmp = r.m_ptr;
        if (tmp != m_ptr)
        {
            if (tmp)
                tmp->add_ref_copy();
            if (m_ptr)
                m_ptr->release();
            m_ptr = tmp;
        }
        return *this;
    }

    void swap(shared_count& r)
    {
        sp_counter_base* tmp = r.m_ptr;
        r.m_ptr = m_ptr;
        m_ptr = tmp;
    }

    long get_use_count() const
    {
        return m_ptr ? m_ptr->get_use_count() : 0;
    }
    
    bool unique() const
    {
        return get_use_count() == 1;
    }

    friend inline bool operator==(const shared_count& a, const shared_count& b)
    {
        return a.m_ptr == b.m_ptr;
    }

private:
    sp_counter_base* m_ptr;
};

template <typename T>
class shared_ptr_base 
{
public:
    using element_type = T;

    constexpr shared_ptr_base()
        : m_ptr(nullptr)
        , m_ref_count()
    {}

    template <typename Y>
    explicit shared_ptr_base(Y* ptr)
        : m_ptr(ptr)
        , m_ref_count(ptr)
    {
        static_assert(sizeof(Y) > 0, "incomplete type");
    }

    // aliasing constructor

    template <typename Y>
    shared_ptr_base(const shared_ptr_base<Y>& r, T* ptr)
        : m_ptr(ptr)
        , m_ref_count(r.m_ref_count)
    {}

    template <typename Y, typename = typename
        std::enable_if<std::is_convertible<Y*, T*>::value>::type>
    shared_ptr_base(const shared_ptr_base<Y>& r)
        : m_ptr(r.m_ptr)
        , m_ref_count(r.m_ref_count)
    {}

    template <typename Y, typename = typename
        std::enable_if<std::is_convertible<Y*, T*>::value>::type>
    shared_ptr_base(shared_ptr_base&& r)
        : m_ptr(r.m_ptr)
        , m_ref_count()
    {
        m_ref_count.swap(r.m_ref_count);
        r.m_ptr = nullptr;
    }

    constexpr shared_ptr_base(std::nullptr_t)
        : m_ptr(nullptr)
        , m_ref_count()
    {}

    template <typename Y>
    shared_ptr_base& operator=(const shared_ptr_base<Y>& r)
    {
        m_ptr = r.m_ptr;
        m_ref_count = r.m_ref_count;
        return *this;
    }
    
    template <typename Y>
    shared_ptr_base& operator=(shared_ptr_base<Y>&& r)
    {
        shared_ptr_base(std::move(r)).swap(*this);
        return *this;
    }

    void reset()
    {
        shared_ptr_base().swap(*this);
    }

    template <typename Y>
    void reset(Y* ptr)
    {
        static_assert(sizeof(Y) != 0, "template parameter should be complete");
        assert(ptr == nullptr || ptr != m_ptr); // exclude self reset except nullptr
        shared_ptr_base(ptr).swap(*this);
    }

    typename std::add_lvalue_reference<T>::type
    operator*() const noexcept
    {
        assert(m_ptr != nullptr);
        return *m_ptr;
    }

    T* operator->() const noexcept
    {
        assert(m_ptr != nullptr);
        return m_ptr;
    }

    T* get() const noexcept
    {
        return m_ptr;
    }

    explicit operator bool() const noexcept
    {
        return m_ptr == nullptr ? false : true;
    }

    bool unique() const
    {
        return m_ref_count.unique();
    }

    long use_count() const
    {
        return m_ref_count.get_use_count();
    }
    
    void swap(shared_ptr_base<T>& other)
    {
        std::swap(m_ptr, other.m_ptr);
        m_ref_count.swap(other.m_ref_count);
    }

private:
    T* m_ptr;
    shared_count m_ref_count;
};

} // namespace lvn

#endif // SHARED_PTR_BASE_H
