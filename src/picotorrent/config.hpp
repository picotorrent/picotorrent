#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "picojson.hpp"

namespace fs = std::experimental::filesystem::v1;

namespace pt
{
    class Environment;

    class Configuration
    {
    public:
        static std::shared_ptr<Configuration> Load(std::shared_ptr<Environment> env);
        static void Save(std::shared_ptr<Environment> env, std::shared_ptr<Configuration> config);

        int CurrentLanguageId();
        void CurrentLanguageId(int id);

        fs::path DefaultSavePath();
        void DefaultSavePath(fs::path path);

    private:
        Configuration(std::shared_ptr<Environment> env,
            std::shared_ptr<picojson::object> obj = std::make_shared<picojson::object>());

        template<typename T> T Get(const char *name, T defaultValue);
        template<> int Get(const char *name, int defaultValue);

        template<typename T> void Set(const char *name, T value);
        template<> void Set(const char *name, int value);

        std::shared_ptr<Environment> m_env;
        std::shared_ptr<picojson::object> m_obj;
    };
}
