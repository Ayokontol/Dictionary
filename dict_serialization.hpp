#pragma once

#include <vector>
#include <map>

#include "dict.hpp"

namespace utils {

    template<class T>
    static void read_(T& data, const any& value);

    template<class T>
    static void read_(std::map<std::string, T>& data, const any& D);

    template<class T>
    static void read_(std::vector<T>& data, const any& D);

    template<class T>
    static void read_(T& data, const any& value) {
        data = value.cast<T>();
    }

    template<class T>
    static void read_(std::map<std::string, T>& data, const any& D) {
        for (auto it : D.cast<dict_t>()) {
            T& tmp = data[it.first];
            read_(tmp, it.second);
        }
    }

    template<class T>
    static void read_(std::vector<T>& data, const any& D) {
        size_t i = 0;
        data.resize(D.cast<dict_t>().size());
        for (auto it : D.cast<dict_t>()) {
            i = std::stoi(it.first);
            read_(data[i], it.second);
        }
    }

    template<class T>
    static void read_(std::map<std::string, T>& data, const dict_t& D) {
        for (auto it : D) {
            T& tmp = data[it.first];
            read_(tmp, it.second);
        }
    }

    template<class T>
    static void read_(std::vector<T>& data, const dict_t& D) {
        size_t i = 0;
        data.resize(D.size());
        for (auto it : D) {
            i = std::stoi(it.first);
            read_(data[i], it.second);
        }
    }

    template<class T>
    T write_(const T& data);

    template<class T>
    dict_t write_(const std::vector<T>& data);

    template<class T>
    dict_t write_(const std::map<std::string, T>& data);

    template<class T>
    dict_t write_(const std::map<std::string, T>& data) {
        dict_t D;
        for (auto it : data)
            put(D, it.first, write_(it.second));
        return D;
    }

    template<class T>
    dict_t write_(const std::vector<T>& data) {
        dict_t D;
        for (size_t i = 0; i < data.size(); ++i)
            put(D, std::to_string(i), write_(data[i]));
        return D;
    }

    template<class T>
    T write_(const T& data) {
        return data;
    }

    template<class T>
    void write(dict_t& D, const T& data) {
        D = write_(data);
    }

    template<class T>
    void read(const dict_t &D, T& data) {
        read_(data, D);
    }
}
