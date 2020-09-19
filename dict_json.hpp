#pragma once

#include "dict.hpp"
#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/istreamwrapper.h"

namespace utils {

    rapidjson::Document to_json(const dict_t& D) {
        rapidjson::Document doc;
        rapidjson::Value json_val;
        auto& allocator = doc.GetAllocator();

        doc.SetObject();

        for (auto it : D) {
            const std::type_info& type = it.second.type_info();
            std::string name = it.first;

            if (type == typeid(bool))
                json_val.SetBool(get<bool>(D, name));
            else if (type == typeid(double))
                json_val.SetDouble(get<double>(D, name));
            else if (type == typeid(float))
                json_val.SetDouble(get<float>(D, name));
            else if (type == typeid(std::string)) {
                std::string tmp = get<std::string>(D, name);
                json_val.SetString(tmp.c_str(), allocator);
            }
            else if (type == typeid(dict_t)) 
                json_val.CopyFrom(to_json(get<dict_t>(D, name)), allocator);
            else if (type == typeid(int))
                json_val.SetInt(get<int>(D, name));
            else if (type == typeid(unsigned int))
                json_val.SetInt(get<unsigned int>(D, name));
            else if (type == typeid(unsigned long))
                json_val.SetInt(get<unsigned long>(D, name));
            else if (type == typeid(long int))
                json_val.SetInt(get<long int>(D, name));
            else if (type == typeid(long long))
                json_val.SetInt(get<long long>(D, name));
            else if (type == typeid(short))
                json_val.SetInt(get<short>(D, name));
            else if (type == typeid(unsigned short))
                json_val.SetInt(get<unsigned short>(D, name));
            else
                throw std::runtime_error("Invalid argument");

            doc.AddMember(rapidjson::Value(name.c_str(), allocator), json_val, allocator);
        }
        return doc;
    }

    void save_to_json(std::ostream& os, const dict_t& D) {
        rapidjson::Document doc = to_json(D);
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        os << buffer.GetString() << std::endl;
    }

    dict_t from_json(const rapidjson::Value& json) {
        dict_t D;

        for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it) {
            auto name = it->name.GetString();

            if (it->value.IsObject())
                put(D, name, from_json(it->value));
            else if (it->value.IsBool())
                put(D, name, it->value.GetBool());
            else if (it->value.IsInt())
                put(D, name, it->value.GetInt());
            else if (it->value.IsDouble())
                put(D, name, it->value.GetDouble());
            else if (it->value.IsString())
                put(D, name, std::string(it->value.GetString()));
            else
                throw std::runtime_error("Invalid argument");
        }
        return D;
    }

    bool load_from_json(std::istream& is, dict_t& D) {
        rapidjson::IStreamWrapper ISW(is);
        rapidjson::Document doc;
        doc.ParseStream(ISW);
        try {
            D = from_json(doc);
        }
        catch (std::exception&) {
            return false;
        }
        return true;
    }
}
