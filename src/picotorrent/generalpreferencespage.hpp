#pragma once

#include <QWidget>

#include <memory>

namespace pt
{
    namespace Ui
    {
        class GeneralPreferencesPage;
    }

    class Configuration;
    class Environment;

    class GeneralPreferencesPage : public QWidget
    {
    public:
        GeneralPreferencesPage(QWidget* parent);
        virtual ~GeneralPreferencesPage();

        void loadConfig(std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env);
        void saveConfig(std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env, bool* requiresRestart);

    private:
        void onShowInNotificationAreaChanged(int state);

        static bool hasCrashReportingConsent(std::shared_ptr<Environment> env);

        Ui::GeneralPreferencesPage* m_ui;
    };
}
