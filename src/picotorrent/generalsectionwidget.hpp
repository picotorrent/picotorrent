#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QComboBox;

namespace pt
{
    class Configuration;

    class GeneralSectionWidget : public QWidget
    {
    public:
        GeneralSectionWidget();
        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg, bool* requiresRestart);

    private:
        void createUi();
        void onShowInNotificationAreaChanged(int state);

        QComboBox* m_languages;
        QCheckBox* m_skipAddTorrentDialog;
        QCheckBox* m_startWithWindows;
        QComboBox* m_startPosition;
        QCheckBox* m_showInNotificationArea;
        QCheckBox* m_minimizeToNotificationArea;
        QCheckBox* m_closeToNotificationArea;
    };
}
