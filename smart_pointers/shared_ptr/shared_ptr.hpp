#pragma once
#ifndef SHARED_PTR_H
#define SHARED_PTR_H

<<<<<<< HEAD
#include "shared_ptr_base.hpp"

namespace lvn
{

// control block (thread safe)
// string ref count
// weak ref count
// shared object
// stored object ptr

// copy_constructible
// copy_assignable
// less_than_comparable
// convertible to bool

template <typename T>
class shared_ptr : public shared_ptr_base<T>
{
public:
// cons
    constexpr shared_ptr() noexcept // 100
        : shared_ptr_base<T>()
    {}

    template <typename Y>
    explicit shared_ptr(Y* ptr) // 109
        : shared_ptr_base<T>(ptr)
    {}

    template <typename Y>
    shared_ptr(const shared_ptr<Y>& r, T* ptr) // 203
        : shared_ptr_base<T>(r, ptr)
    {}
    
    template <typename Y, typename = typename 
        std::enable_if<std::is_convertible<Y*, T*>::value>::type>
    shared_ptr(const shared_ptr<Y>& r) noexcept // 216
        : shared_ptr_base<T>(r)
    {}

    template <typename Y, typename = typename
        std::enable_if<std::is_convertible<T*, Y*>::value>::type>
    shared_ptr(shared_ptr<Y>&& r)
        : shared_ptr_base<T>(std::move(r)) // 234
    {}

    constexpr shared_ptr(std::nullptr_t ptr) noexcept // 264
        : shared_ptr_base<T>(ptr)
    {}
    template <typename Y>
    shared_ptr& operator=(const shared_ptr<Y>& r) noexcept // 267
    {
        this->shared_ptr_base<T>::operator=(r);
        return *this;
    }

    template <typename Y>
    shared_ptr& operator=(shared_ptr<Y>&& r) noexcept // 292
    {
        this->shared_ptr_base<Y>::operator=(std::move(r));
        return *this;
    }  
};

} // namespace lvn

#endif // SHARED_PTR_H
