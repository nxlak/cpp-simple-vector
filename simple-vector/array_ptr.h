// вставьте сюда ваш код для класса ArrayPtr
// внесите в него изменения, 
// которые позволят реализовать move-семантику

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(std::size_t size) {
        if(size == 0){
            raw_ptr_ = nullptr;
        } else {
            raw_ptr_ = new Type[size];
        } 
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept 
        : raw_ptr_(raw_ptr){
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other){
        ArrayPtr tmp(std::exchange(other.raw_ptr_, nullptr));
        swap(tmp);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other){
        if(this != &other){
            ArrayPtr tmp(std::exchange(other.raw_ptr_, nullptr));
            swap(tmp);
        }
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* new_raw_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return new_raw_ptr;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](std::size_t index) noexcept {
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](std::size_t index) const noexcept {
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};
