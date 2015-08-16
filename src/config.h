#ifndef _PT_CONFIG_H
#define _PT_CONFIG_H

#include <rapidjson/document.h>
#include <string>

class Config
{
public:
    static Config& GetInstance()
    {
        static Config instance;
        return instance;
    }

    std::string GetPyPath();
    std::string GetPyRuntimePath();

    void Save();

private:
    Config();

    void Load();

    rapidjson::Value::MemberIterator FindOrDefault(const char* key);
    rapidjson::Value::MemberIterator FindOrDefault(const char* key, rapidjson::Document& doc);

    Config(Config const&);
    void operator=(Config const&);

    rapidjson::Document doc_;
    rapidjson::Document default_;
    static std::string file_;
};

#endif
