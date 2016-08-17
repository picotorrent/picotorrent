#pragma once

#include <map>
#include <memory>
#include <string>

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

class Configuration
{
public:
    enum StartupPosition
    {
        Normal,
        Minimized,
        Hidden
    };

    class Section
    {
        friend class Configuration;

    protected:
        Section(const std::shared_ptr<picojson::object>& part);

        bool Get(const char *part, const char* key, bool default_value);
        int Get(const char *part, const char* key, int default_value);
        std::string Get(const char *part, const char* key, const std::string &default_value);

        void Set(const char *part, const char* key, bool value);
        void Set(const char *part, const char* key, int value);
        void Set(const char *part, const char* key, const std::string &value);

    private:
        std::shared_ptr<picojson::object> m_part;
    };

    struct SessionSection : public Section
    {
        friend class Configuration;

        int GetActiveChecking();
        int GetActiveDhtLimit();
        int GetActiveDownloads();
        int GetActiveLimit();
        int GetActiveLoadedLimit();
        int GetActiveLsdLimit();
        int GetActiveSeeds();
        int GetActiveTrackerLimit();

        int GetDownloadRateLimit();
        void SetDownloadRateLimit(int limit);

        int GetStopTrackerTimeout();

        int GetUploadRateLimit();
        void SetUploadRateLimit(int limit);

    protected:
        using Section::Section;
    };

    static Configuration& GetInstance();

    // Sections
    std::shared_ptr<SessionSection> Session();

    int GetCurrentLanguageId();
    void SetCurrentLanguageId(int languageId);

    std::string GetDefaultSavePath();
    void SetDefaultSavePath(const std::string& path);

    StartupPosition GetStartupPosition();
    void SetStartupPosition(StartupPosition pos);

private:
    Configuration();
    ~Configuration();

    template<typename T> T Get(const char *name, T defaultValue);
    template<> int Get(const char *name, int defaultValue);

    template<typename T> void Set(const char *name, T value);
    template<> void Set(const char *name, int value);

    void Load();
    void Save();

    std::shared_ptr<picojson::object> m_cfg;
};
