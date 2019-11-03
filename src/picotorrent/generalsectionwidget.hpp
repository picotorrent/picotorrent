#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QComboBox;

namespace pt
{
    class Configuration;
    class Environment;

    class GeneralSectionWidget : public QWidget
    {
    public:
        GeneralSectionWidget();
        void loadConfig(std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env);
        void saveConfig(std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env, bool* requiresRestart);

    private:
        void createUi();
        void onShowInNotificationAreaChanged(int state);

        static bool hasCrashReportingConsent(std::shared_ptr<Environment> env);

        QComboBox* m_languages;
        QCheckBox* m_skipAddTorrentDialog;
        QCheckBox* m_startWithWindows;
        QComboBox* m_startPosition;
        QCheckBox* m_showInNotificationArea;
        QCheckBox* m_minimizeToNotificationArea;
        QCheckBox* m_closeToNotificationArea;
        QCheckBox* m_automaticCrashReportingEnabled;
        QCheckBox* m_geoipEnabled;
    };
}
