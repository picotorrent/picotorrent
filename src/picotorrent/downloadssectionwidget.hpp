#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QLineEdit;
class QPushButton;

namespace pt
{
    class Configuration;

    class DownloadsSectionWidget : public QWidget
    {
    public:
        DownloadsSectionWidget();
        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void createUi();
        void showPathDialog(QLineEdit* target);

        QLineEdit* m_savePath;
        QPushButton* m_savePathBrowse;
        QCheckBox* m_moveCompleted;
        QLineEdit* m_moveCompletedPath;
        QPushButton* m_moveCompletedBrowse;
        QCheckBox* m_onlyMoveFromDefault;
        QLineEdit* m_downLimit;
        QCheckBox* m_downLimitEnable;
        QLineEdit* m_upLimit;
        QCheckBox* m_upLimitEnable;
        QLineEdit* m_totalActive;
        QLineEdit* m_activeDownloads;
        QLineEdit* m_activeSeeds;
    };
}
