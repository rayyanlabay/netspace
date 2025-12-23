
#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <memory>
#include <unordered_map>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

#include "singleton.hpp"

namespace abc
{
    class Config
    {

    public:
        virtual ~Config() noexcept {};

        Config(const Config &other_) = delete;
        Config &operator=(const Config &other_) = delete;

        void SetConfig(const std::string &path_)
        {
            std::ifstream f(path_.c_str());
            m_json = json::parse(f);
        };

        class ConfigProxy
        {
        public:
            explicit ConfigProxy(const json &config_json_, const char *key_) : m_config(config_json_), m_key(key_){};
            // Using generated CCtor and Dtor
            ConfigProxy operator[](const char *key_) { return ConfigProxy(m_config[m_key], key_); };
            operator std::string() const { return m_config[m_key]; };

        private:
            const json m_config;
            const char *m_key;
        };

        ConfigProxy operator[](const char *key_) const
        {
            return ConfigProxy(m_json, key_);
        };

    private:
        json m_json;

        Config() : m_json(){}; // private for collaboration with singleton
        friend class Singleton<Config>;
    };

} // namespace abc

#endif //__I_CONFIG_HPP__
