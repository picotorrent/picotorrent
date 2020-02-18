#pragma once

#include <QWidget>

#include <memory>

class QLineEdit;

namespace pt
{
    namespace Ui
    {
        class DownloadsPreferencesPage;
    }

    class Configuration;

    class DownloadsPreferencesPage : public QWidget
    {
    public:
        DownloadsPreferencesPage(QWidget* parent);
        virtual ~DownloadsPreferencesPage();

        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void showPathDialog(QLineEdit* target);

        Ui::DownloadsPreferencesPage* m_ui;
    };
}
