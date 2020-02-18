#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;

namespace Ui
{
    class ConnectionPreferencesPage;
}

namespace pt
{
    class Configuration;

    class ConnectionPreferencesPage : public QWidget
    {
    public:
        ConnectionPreferencesPage(QWidget* parent);
        ~ConnectionPreferencesPage();

        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void setupNetworkAdapters();

        Ui::ConnectionPreferencesPage* m_ui;
    };
}
