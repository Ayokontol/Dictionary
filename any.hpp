#pragma once

#include <typeinfo>
#include <stdexcept>
#include <initializer_list>
#include <vector>

namespace utils {
    class no_key_exception : std::exception {
        const char* what_;

    public:
        no_key_exception(const char* what_arg = "No key") : what_(what_arg) {
        }

        const char* what() const noexcept override {
            return what_;
        }
    };

    class invalid_type_exception : std::exception {
        const char* what_;

    public:
        invalid_type_exception(const char* what_arg = "Invalid type") : what_(what_arg) {
        }
        const char* what() const noexcept override {
            return what_;
        }
    };

    class any
    {
    public:
        any() : held_(nullptr) {}

        any(const any& other) {
            held_ = (other.held_ ? other.held_->clone() : nullptr);
        }

        any(any&& other) noexcept {
            held_ = other.held_;
            other.held_ = nullptr;
        }

        template<class T>
        any(const T& data) : held_(new holder<T>(data)) {}

        template<class T>
        any(T&& data) : held_(new holder<std::decay_t<T>>(std::forward<T>(data))) {}

        any& swap(any& other) {
            std::swap(held_, other.held_);
            return *this;
        }

        any& operator=(any&& other) noexcept {
            if (held_)
                delete held_;
            held_ = other.held_;
            other.held_ = nullptr;
            return *this;
        }

        bool operator==(const any& other) const {
            return *other.held_ == *held_;
        }

        bool empty() const {
            return held_ == nullptr;
        }

        const std::type_info& type_info() const {
            if (held_)
                return held_->type_info();
            else
                throw std::runtime_error("Invalid argument");
        }

        void clear() {
            if (held_)
                delete held_; //NOLINT
            held_ = nullptr;
        }

        ~any() {
            clear();
        }

        template<typename U>
        U cast() const {
            if (held_ == nullptr || typeid(U) != held_->type_info())
                throw invalid_type_exception();
            return dynamic_cast<holder<U>*>(held_)->data_;
        }

        template<class U>
        const U& get_ref() const {
            if (held_ == nullptr || typeid(U) != held_->type_info() || std::is_const<U>::value != held_->is_const())
                throw invalid_type_exception();
            return dynamic_cast<holder<U>&>(*held_).data_;
        }

        template<class U>
        const U* get_ptr() const {
            if (held_ == nullptr || typeid(U) != held_->type_info() || std::is_const<U>::value != held_->is_const())
                return nullptr;
            return &(dynamic_cast<holder<U>*>(held_)->data_);
        }

        template<class U>
        U& get_ref() {
            if (held_ == nullptr || typeid(U) != held_->type_info() || std::is_const<U>::value != held_->is_const())
                throw invalid_type_exception();
            return dynamic_cast<holder<U>&>(*held_).data_;
        }

        template<class U>
        U* get_ptr() {
            if (held_ == nullptr || typeid(U) != held_->type_info() || std::is_const<U>::value != held_->is_const())
                return nullptr;
            return &(dynamic_cast<holder<U>*>(held_)->data_);
        }

    private:
        struct base_holder
        {
            virtual ~base_holder() {}
            virtual base_holder* clone() const = 0;
            virtual const std::type_info& type_info() const = 0;
            virtual bool is_const() const = 0;
            virtual bool operator==(const base_holder& other) const = 0;
        };

        template<class V>
        struct holder : base_holder
        {
            holder(const V& data) : data_(data) {}

            holder(V&& data) : data_(std::forward<V>(data)) {}

            const std::type_info& type_info() const override {
                return typeid(data_);
            }
            base_holder* clone() const override {
                return new holder(data_);
            }

            bool is_const() const override {
                return std::is_const<V>::value;
            }

            bool operator==(const base_holder& other) const override {
                if (typeid(V) == other.type_info()) {
                    const holder<V>& tmp = dynamic_cast<const holder<V>&>(other);
                    return tmp.data_ == data_;
                }
                else {
                    return false;
                }
            }
            V data_;
        };
    private:
        base_holder* held_;
    };
}
