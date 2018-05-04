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
        enum ConnectionProxyType
        {
            None,
            SOCKS4,
            SOCKS5,
            SOCKS5_Password,
            HTTP,
            HTTP_Password,
            I2P
        };

        enum WindowState
        {
            Normal,
            Minimized,
            Hidden,
            Maximized
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

            int ActiveChecking();
            int ActiveDhtLimit();
            int ActiveDownloads();
            void ActiveDownloads(int value);
            int ActiveLimit();
            void ActiveLimit(int value);
            int ActiveLsdLimit();
            int ActiveSeeds();
            void ActiveSeeds(int value);
            int ActiveTrackerLimit();

            bool EnableAnonymousMode();
            void EnableAnonymousMode(bool value);
            bool EnableDht();
            void EnableDht(bool value);
            bool EnableLsd();
            void EnableLsd(bool value);
            bool EnablePex();
            void EnablePex(bool value);

            bool RequireIncomingEncryption();
            void RequireIncomingEncryption(bool value);
            bool RequireOutgoingEncryption();
            void RequireOutgoingEncryption(bool value);

            bool EnableDownloadRateLimit();
            void EnableDownloadRateLimit(bool value);
            int DownloadRateLimit();
            void DownloadRateLimit(int limit);

            int StopTrackerTimeout();

            bool EnableUploadRateLimit();
            void EnableUploadRateLimit(bool value);
            int UploadRateLimit();
            void UploadRateLimit(int limit);

        protected:
            using Section::Section;
        };

        struct UISection : public Section
        {
            friend class Configuration;

            bool SkipAddTorrentDialog();
            void SkipAddTorrentDialog(bool value);
            bool ShowInNotificationArea();
            void ShowInNotificationArea(bool value);
            bool CloseToNotificationArea();
            void CloseToNotificationArea(bool value);
            bool MinimizeToNotificationArea();
            void MinimizeToNotificationArea(bool value);

        protected:
            using Section::Section;
        };

        static std::shared_ptr<Configuration> Load(std::shared_ptr<Environment> env, std::string& error);
        static void Save(std::shared_ptr<Environment> env, std::shared_ptr<Configuration> config);

        std::shared_ptr<SessionSection> Session();
        std::shared_ptr<UISection> UI();

        int CurrentLanguageId();
        void CurrentLanguageId(int id);

        fs::path LanguagesPath();

        bool EncryptConfigurationFile();
        void EncryptConfigurationFile(bool val);

        fs::path DefaultSavePath();
        void DefaultSavePath(fs::path path);

        std::string IgnoredVersion();
        void IgnoredVersion(std::string const& version);

        std::vector<std::pair<std::string, int>> ListenInterfaces();
        void ListenInterfaces(const std::vector<std::pair<std::string, int>>& interfaces);

        bool MoveCompletedDownloads();
        void MoveCompletedDownloads(bool enable);
        fs::path MoveCompletedDownloadsPath();
        void MoveCompletedDownloadsPath(fs::path path);
        bool MoveCompletedDownloadsFromDefaultOnly();
        void MoveCompletedDownloadsFromDefaultOnly(bool enable);

        ConnectionProxyType ProxyType();
        void ProxyType(ConnectionProxyType type);

        std::string ProxyHost();
        void ProxyHost(const std::string &host);

        int ProxyPort();
        void ProxyPort(int port);

        std::string ProxyUsername();
        void ProxyUsername(const std::string &user);

        std::string ProxyPassword();
        void ProxyPassword(const std::string &pass);

        bool ProxyForce();
        void ProxyForce(bool value);

        bool ProxyHostnames();
        void ProxyHostnames(bool value);

        bool ProxyPeers();
        void ProxyPeers(bool value);

        bool ProxyTrackers();
        void ProxyTrackers(bool value);

        std::string UpdateUrl();

        WindowState StartPosition();
        void StartPosition(WindowState state);

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
