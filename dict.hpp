#pragma once
#include <unordered_map>
#include <string>
#include <typeinfo>

#include "any.hpp"

namespace utils {

    class dict_t {
        std::unordered_map<std::string, any> M_;

    public:
        dict_t() {}

    private:
        struct init {
            std::string name;
            any data;

            init(const std::string& name, const dict_t& data) : name(name), data(data) {
            }

            template<class T>
            init(const std::string& name, const T& data) : name(name), data(data) {
            }
        };
    public:
        dict_t(const std::initializer_list<init>& list) {
            for (auto it : list)
                put(it.name, std::move(it.data));
        }

        dict_t& operator=(const dict_t& other) {
            M_ = other.M_;
            return *this;
        }

        dict_t& operator=(dict_t&& other) noexcept {
            M_ = std::move(other.M_);
            return *this;
        }

        dict_t(const dict_t& other) : M_(other.M_) {}

        dict_t(dict_t&& other) noexcept : M_(std::move(other.M_)) {}

        auto begin() const {
            return M_.begin();
        }

        auto end() const {
            return M_.end();
        }

        template<class T>
        bool put(const std::string& key, T&& value) {
            auto tmp = M_.emplace(key, std::forward<T>(value));
            return tmp.second;
        }

        const any& get(const std::string& key) const {
            try {
                return M_.at(key);
            }
            catch (std::exception&) {
                throw no_key_exception();
            }
        }

        any& get(const std::string& key) {
            try {
                return M_.at(key);
            }
            catch (std::exception&) {
                throw no_key_exception();
            }
        }

        auto find(const std::string& key) const {
            return M_.find(key);
        }

        bool remove(const std::string& key) {
            return M_.erase(key);
        }

        bool contains(const std::string& key) const {
            return M_.find(key) != M_.end();
        }

        bool empty() const {
            return M_.empty();
        }

        size_t size() const {
            return M_.size();
        }

        void clear() {
            M_.clear();
        }

        bool is_dict(const std::string& key) const {
            auto it = M_.find(key);
            if (it != M_.end() && it->second.type_info() == typeid(dict_t))
                return true;
            return false;
        }

        bool operator==(const dict_t& other) const {
            return M_ == other.M_;
        }

        bool operator!=(const dict_t& other) const {
            return !(*this == other);
        }

        ~dict_t() {
            if (!empty())
                clear();
        }
    };

    template<class T>
    bool put(dict_t& D, const std::string& key, T&& value) {
        return D.put(key, std::forward<T>(value));
    }

    template<class T>
    const T& get(const dict_t &D, const std::string& key) {
        try {
            return D.get(key).get_ref<T>();
        }
        catch (std::exception&) {
            throw;
        }
    }

    template<class T>
    T& get(dict_t& D, const std::string& key) {
        try {
            return D.get(key).get_ref<T>();
        }
        catch (std::exception&) {
            throw;
        }
    }

    template<class T>
    T* get_ptr(dict_t &D, const std::string& key) {
        if (!D.contains(key))
            return nullptr;
        return D.get(key).get_ptr<T>();
    }

    template<class T>
    const T* get_ptr(const dict_t& D, const std::string& key) {
        if (!D.contains(key))
            return nullptr;
        return D.get(key).get_ptr<T>();
    }

    inline bool remove(dict_t& D, const std::string& key) {
        return D.remove(key);
    }

    inline bool contains(const dict_t& D, const std::string& key) {
        return D.contains(key);
    }

    inline bool empty(dict_t& D) {
        return D.empty();
    }

    inline void clear(dict_t& D) {
        D.clear();
    }

    inline bool is_dict(const dict_t& D, const std::string& key) {
        return D.is_dict(key);
    }
}
