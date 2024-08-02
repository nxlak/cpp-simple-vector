// вставьте сюда ваш код для класса SimpleVector
// внесите необходимые изменения для поддержки move-семантики

#pragma once

#include "array_ptr.h"

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <utility>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t new_capacity) : capacity_(new_capacity) {
    }

    size_t GetCapacity() {
        return capacity_;
    }

private:
    size_t capacity_;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(std::size_t size) 
        : SimpleVector(size, Type{}){
    }

    SimpleVector(size_t size, const Type& value) : items_(size), size_(size), capacity_(size) {
        for (auto it = begin(); it != end(); ++it) {
                *it = value;
            }
    }

    SimpleVector(std::initializer_list<Type> init) : items_(init.size()),size_(init.size()), capacity_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) : items_(other.size_), size_(other.size_), capacity_(other.capacity_) {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if(this != &rhs){
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector(SimpleVector&& other) noexcept : items_(std::move(other.items_)), size_(std::exchange(other.size_, 0)), capacity_(std::exchange(other.capacity_, 0)) {}

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            items_.swap(rhs.items_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    SimpleVector(ReserveProxyObj other) : capacity_(other.GetCapacity()) {
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("element not exist");
        } else {
            return items_[index];
        }
    }

    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("element not exist");
        } else {
            return items_[index];
        }
    }

    void PushBack(const Type& item) {
        if (capacity_ >= size_ + 1) {
            items_[size_] = item;
            ++size_;
        } else if (capacity_ == 0 && size_ == 0) {
            ArrayPtr<Type> tmp_items(1);
            tmp_items[0] = item;
            items_.swap(tmp_items);
            ++size_;
            ++capacity_;
        } else {
            ArrayPtr<Type> tmp_items(capacity_ * 2);
            std::copy(begin(), end(), tmp_items.Get());
            tmp_items[size_] = item;
            items_.swap(tmp_items);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PushBack(Type&& item) {
        if (capacity_ >= size_ + 1) {
            items_[size_] = std::move(item);
            ++size_;
        } else if (capacity_ == 0 && size_ == 0) {
            ArrayPtr<Type> tmp_items(1);
            tmp_items[0] = std::move(item);
            items_.swap(tmp_items);
            ++size_;
            ++capacity_;
        } else {
            ArrayPtr<Type> tmp_items(capacity_ * 2);
            std::move(begin(), end(), tmp_items.Get());
            tmp_items[size_] = std::move(item);
            items_.swap(tmp_items);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PopBack() noexcept {
        assert(size_ != 0);
        --size_;
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            ArrayPtr<Type> tmp_items(new_size);
            std::move(begin(), end(), tmp_items.Get());
    
            for (auto it = tmp_items.Get() + size_; it != tmp_items.Get() + new_size; ++it) {
                *it = Type{}; // Initialize with default value
            }
    
            items_.swap(tmp_items);
            size_ = new_size;
            capacity_ = new_size;
        } else if (new_size < size_) {
            for (auto it = begin() + new_size; it != end(); ++it) {
                *it = Type{}; // Initialize with default value
            }
            
            size_ = new_size;
        } else {
            for (auto it = begin() + new_size; it != end(); --it) {
                *it = Type{}; // Initialize with default value
            }
            
            size_ = new_size;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp_items(new_capacity);
            std::move(begin(), end(), tmp_items.Get());
            items_.swap(tmp_items);
            capacity_ = new_capacity;
        }
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos <= end());
        size_t needed_pos = pos - begin();
    
        if (capacity_ >= size_ + 1) {
            std::copy_backward(begin() + needed_pos, end(), end() + 1);
            items_[needed_pos] = value;
            ++size_;
            return begin() + needed_pos;
        } else if (capacity_ == 0 && size_ == 0) {
            ArrayPtr<Type> tmp_items(1);
            tmp_items = value;
            items_.swap(tmp_items);
            ++size_;
            ++capacity_;
            return begin();
        } else {
            ArrayPtr<Type> tmp_items(capacity_ * 2);
            std::copy(begin(), end(), tmp_items.Get());
                std::copy_backward(tmp_items.Get() + needed_pos, tmp_items.Get() + size_, tmp_items.Get() + size_ + 1);
            tmp_items[needed_pos] = value;
            items_.swap(tmp_items);
            ++size_;
            capacity_ *= 2;
            return begin() + needed_pos;
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        size_t needed_pos = pos - begin();
 
        if (capacity_ >= size_ + 1) {
            std::move(begin() + needed_pos, end(), begin() + needed_pos + 1);
            items_[needed_pos] = std::move(value);
            ++size_;
            return begin() + needed_pos;
        } else if (capacity_ == 0 && size_ == 0) {
            ArrayPtr<Type> tmp_items(1);
 
            tmp_items[0] = std::move(value);
            items_.swap(tmp_items);
            ++size_;
            ++capacity_;
            return begin();
        } else {
            ArrayPtr<Type> tmp_items(capacity_ * 2);
 
            std::move(begin(), end(), tmp_items.Get());
            std::move_backward(tmp_items.Get() + needed_pos, tmp_items.Get() + size_, tmp_items.Get() + size_ + 1);
            tmp_items[needed_pos] = std::move(value);
            items_.swap(tmp_items);
            ++size_;
            capacity_ *= 2;
            return begin() + needed_pos;
        }
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        size_t needed_pos = pos - items_.Get();
    
        std::move(begin() + needed_pos + 1, end(), begin() + needed_pos);
        --size_;
        return begin() + needed_pos;
    }

    Iterator begin() noexcept {
        return Iterator{items_.Get()};
    }
    
    Iterator end() noexcept {
        return Iterator{items_.Get() + size_};
    }
    
    ConstIterator begin() const noexcept {
        return ConstIterator{items_.Get()};
    }
    
    ConstIterator end() const noexcept {
        return ConstIterator{items_.Get() + size_};
    }
    
    ConstIterator cbegin() const noexcept {
        return ConstIterator{items_.Get()};
    }
    
    ConstIterator cend() const noexcept {
        return ConstIterator{items_.Get() + size_};
    }
    
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
} 

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
} 
