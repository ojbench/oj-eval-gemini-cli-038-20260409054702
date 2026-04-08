#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <type_traits>
#include <cstdlib>

namespace sjtu {
    template<typename T>
    class vector {
    private:
        T* data_;
        size_t size_;
        size_t capacity_;

        void reallocate(size_t new_cap) {
            if (new_cap == 0) {
                if (data_) {
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        for (size_t i = 0; i < size_; ++i) {
                            data_[i].~T();
                        }
                    }
                    std::free(data_);
                    data_ = nullptr;
                }
                capacity_ = 0;
                return;
            }
            
            if constexpr (std::is_trivially_copyable_v<T>) {
                T* new_data = static_cast<T*>(std::realloc(data_, new_cap * sizeof(T)));
                if (!new_data) throw std::bad_alloc();
                data_ = new_data;
                capacity_ = new_cap;
            } else {
                T* new_data = static_cast<T*>(std::malloc(new_cap * sizeof(T)));
                if (!new_data) throw std::bad_alloc();
                for (size_t i = 0; i < size_; ++i) {
                    new (&new_data[i]) T(std::move(data_[i]));
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        data_[i].~T();
                    }
                }
                if (data_) {
                    std::free(data_);
                }
                data_ = new_data;
                capacity_ = new_cap;
            }
        }

    public:
        vector() : data_(nullptr), size_(0), capacity_(0) {}
        
        vector(const vector &other) : data_(nullptr), size_(other.size_), capacity_(other.size_) {
            if (capacity_ > 0) {
                data_ = static_cast<T*>(std::malloc(capacity_ * sizeof(T)));
                if (!data_) throw std::bad_alloc();
                if constexpr (std::is_trivially_copyable_v<T>) {
                    std::memcpy(data_, other.data_, size_ * sizeof(T));
                } else {
                    for (size_t i = 0; i < size_; ++i) {
                        new (&data_[i]) T(other.data_[i]);
                    }
                }
            }
        }
        
        vector(vector &&other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        
        ~vector() {
            if (data_) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_t i = 0; i < size_; ++i) {
                        data_[i].~T();
                    }
                }
                std::free(data_);
            }
        }
        
        vector &operator=(const vector &other) {
            if (this == &other) return *this;
            if (data_) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_t i = 0; i < size_; ++i) {
                        data_[i].~T();
                    }
                }
                std::free(data_);
                data_ = nullptr;
            }
            size_ = other.size_;
            capacity_ = other.size_;
            if (capacity_ > 0) {
                data_ = static_cast<T*>(std::malloc(capacity_ * sizeof(T)));
                if (!data_) throw std::bad_alloc();
                if constexpr (std::is_trivially_copyable_v<T>) {
                    std::memcpy(data_, other.data_, size_ * sizeof(T));
                } else {
                    for (size_t i = 0; i < size_; ++i) {
                        new (&data_[i]) T(other.data_[i]);
                    }
                }
            }
            return *this;
        }
        
        vector &operator=(vector &&other) noexcept {
            if (this == &other) return *this;
            if (data_) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    for (size_t i = 0; i < size_; ++i) {
                        data_[i].~T();
                    }
                }
                std::free(data_);
            }
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
            return *this;
        }
        
        T &at(const size_t &pos) {
            if (pos >= size_) throw std::out_of_range("out of range");
            return data_[pos];
        }
        
        const T &at(const size_t &pos) const {
            if (pos >= size_) throw std::out_of_range("out of range");
            return data_[pos];
        }
        
        T &operator[](const size_t &pos) { return data_[pos]; }
        const T &operator[](const size_t &pos) const { return data_[pos]; }
        
        const T &front() const { return data_[0]; }
        const T &back() const { return data_[size_ - 1]; }
        
        class iterator {
        public:
            T* ptr;
            iterator() : ptr(nullptr) {}
            iterator(T* p) : ptr(p) {}
            iterator(const iterator &other) : ptr(other.ptr) {}
            iterator operator+(const int &n) const { return iterator(ptr + n); }
            iterator operator-(const int &n) const { return iterator(ptr - n); }
            int operator-(const iterator &rhs) const { return ptr - rhs.ptr; }
            iterator& operator+=(const int &n) { ptr += n; return *this; }
            iterator& operator-=(const int &n) { ptr -= n; return *this; }
            iterator operator++(int) { iterator tmp = *this; ++ptr; return tmp; }
            iterator& operator++() { ++ptr; return *this; }
            iterator operator--(int) { iterator tmp = *this; --ptr; return tmp; }
            iterator& operator--() { --ptr; return *this; }
            T& operator*() const { return *ptr; }
            T* operator->() const { return ptr; }
            bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
            bool operator==(const class const_iterator &rhs) const { return ptr == rhs.ptr; }
            bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
            bool operator!=(const class const_iterator &rhs) const { return ptr != rhs.ptr; }
        };
        
        class const_iterator {
        public:
            const T* ptr;
            const_iterator() : ptr(nullptr) {}
            const_iterator(const T* p) : ptr(p) {}
            const_iterator(const const_iterator &other) : ptr(other.ptr) {}
            const_iterator(const iterator &other) : ptr(other.ptr) {}
            const_iterator operator+(const int &n) const { return const_iterator(ptr + n); }
            const_iterator operator-(const int &n) const { return const_iterator(ptr - n); }
            int operator-(const const_iterator &rhs) const { return ptr - rhs.ptr; }
            const_iterator& operator+=(const int &n) { ptr += n; return *this; }
            const_iterator& operator-=(const int &n) { ptr -= n; return *this; }
            const_iterator operator++(int) { const_iterator tmp = *this; ++ptr; return tmp; }
            const_iterator& operator++() { ++ptr; return *this; }
            const_iterator operator--(int) { const_iterator tmp = *this; --ptr; return tmp; }
            const_iterator& operator--() { --ptr; return *this; }
            const T& operator*() const { return *ptr; }
            const T* operator->() const { return ptr; }
            bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
            bool operator==(const const_iterator &rhs) const { return ptr == rhs.ptr; }
            bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
            bool operator!=(const const_iterator &rhs) const { return ptr != rhs.ptr; }
        };
        
        iterator begin() { return iterator(data_); }
        const_iterator cbegin() const { return const_iterator(data_); }
        iterator end() { return iterator(data_ + size_); }
        const_iterator cend() const { return const_iterator(data_ + size_); }
        
        bool empty() const { return size_ == 0; }
        size_t size() const { return size_; }
        size_t capacity() const { return capacity_; }
        
        void clear() {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (size_t i = 0; i < size_; ++i) {
                    data_[i].~T();
                }
            }
            size_ = 0;
        }
        
        iterator insert(iterator pos, const T &value) {
            size_t ind = pos.ptr - data_;
            if (size_ == capacity_) {
                reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
            }
            T* p = data_ + ind;
            if constexpr (std::is_trivially_copyable_v<T>) {
                std::memmove(p + 1, p, (size_ - ind) * sizeof(T));
            } else {
                if (size_ > ind) {
                    new (&data_[size_]) T(std::move(data_[size_ - 1]));
                    for (size_t i = size_ - 1; i > ind; --i) {
                        data_[i] = std::move(data_[i - 1]);
                    }
                    data_[ind] = value;
                } else {
                    new (p) T(value);
                }
            }
            if constexpr (std::is_trivially_copyable_v<T>) {
                new (p) T(value);
            }
            ++size_;
            return iterator(data_ + ind);
        }
        
        iterator insert(const size_t &ind, const T &value) {
            return insert(iterator(data_ + ind), value);
        }
        
        iterator erase(iterator pos) {
            size_t ind = pos.ptr - data_;
            T* p = data_ + ind;
            if constexpr (std::is_trivially_copyable_v<T>) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    p->~T();
                }
                std::memmove(p, p + 1, (size_ - ind - 1) * sizeof(T));
            } else {
                for (size_t i = ind; i < size_ - 1; ++i) {
                    data_[i] = std::move(data_[i + 1]);
                }
                data_[size_ - 1].~T();
            }
            --size_;
            return iterator(p);
        }
        
        iterator erase(const size_t &ind) {
            return erase(iterator(data_ + ind));
        }
        
        void push_back(const T &value) {
            if (size_ == capacity_) {
                reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
            }
            new (&data_[size_]) T(value);
            ++size_;
        }
        
        void push_back(T &&value) {
            if (size_ == capacity_) {
                reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
            }
            new (&data_[size_]) T(std::move(value));
            ++size_;
        }
        
        void pop_back() {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                data_[size_ - 1].~T();
            }
            --size_;
        }
    };
}

#endif
