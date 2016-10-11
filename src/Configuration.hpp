#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace picojson
{
    class value;
    typedef std::map<std::string, value> object;
}

class Configuration
{
public:
    enum CloseAction
    {
        Prompt,
        Close,
        Minimize
    };

    enum ProxyType
    {
        None,
        SOCKS4,
        SOCKS5,
        SOCKS5_Password,
        HTTP,
        HTTP_Password,
        I2P
    };

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

        bool GetRequireIncomingEncryption();
        void SetRequireIncomingEncryption(bool value);
        bool GetRequireOutgoingEncryption();
        void SetRequireOutgoingEncryption(bool value);

        int GetDownloadRateLimit();
        void SetDownloadRateLimit(int limit);

        int GetStopTrackerTimeout();

        int GetUploadRateLimit();
        void SetUploadRateLimit(int limit);

    protected:
        using Section::Section;
    };

    struct UISection : public Section
    {
        friend class Configuration;

        bool GetShowInNotificationArea();
        void SetShowInNotificationArea(bool value);
        bool GetCloseToNotificationArea();
        void SetCloseToNotificationArea(bool value);
        bool GetMinimizeToNotificationArea();
        void SetMinimizeToNotificationArea(bool value);

    protected:
        using Section::Section;
    };

    static Configuration& GetInstance();
    std::shared_ptr<picojson::object> GetRawObject();

    // Sections
    std::shared_ptr<SessionSection> Session();
    std::shared_ptr<UISection> UI();

    CloseAction GetCloseAction();
    void SetCloseAction(CloseAction action);

    int GetCurrentLanguageId();
    void SetCurrentLanguageId(int languageId);

    std::string GetDefaultSavePath();
    void SetDefaultSavePath(const std::string& path);

    bool GetMoveCompletedDownloads();
    void SetMoveCompletedDownloads(bool value);

    std::string GetMoveCompletedDownloadsPath();
    void SetMoveCompletedDownloadsPath(const std::string& path);

    bool GetMoveCompletedDownloadsFromDefaultOnly();
    void SetMoveCompletedDownloadsFromDefaultOnly(bool value);

    std::vector<std::pair<std::string, int>> GetListenInterfaces();
    void SetListenInterfaces(const std::vector<std::pair<std::string, int>>& interfaces);

    bool GetPromptForRemovingData();
    void SetPromptForRemovingData(bool val);

    ProxyType GetProxyType();
    void SetProxyType(ProxyType type);

    std::string GetProxyHost();
    void SetProxyHost(const std::string &host);

    int GetProxyPort();
    void SetProxyPort(int port);

    std::string GetProxyUsername();
    void SetProxyUsername(const std::string &user);

    std::string GetProxyPassword();
    void SetProxyPassword(const std::string &pass);

    bool GetProxyForce();
    void SetProxyForce(bool value);

    bool GetProxyHostnames();
    void SetProxyHostnames(bool value);

    bool GetProxyPeers();
    void SetProxyPeers(bool value);

    bool GetProxyTrackers();
    void SetProxyTrackers(bool value);

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
