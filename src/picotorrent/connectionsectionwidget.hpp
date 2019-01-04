#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;

namespace pt
{
    class Configuration;

    class ConnectionSectionWidget : public QWidget
    {
    public:
        ConnectionSectionWidget();
        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void createUi();
        void onListenInterfaceChanged(int index);
        void setupNetworkAdapters();

        QComboBox* m_listenInterfaces;
        QLineEdit* m_port;
        QLabel* m_ipv4;
        QLabel* m_ipv6;
        QCheckBox* m_encryptOutgoing;
        QCheckBox* m_encryptIncoming;
        QCheckBox* m_enableDht;
        QCheckBox* m_enableLsd;
        QCheckBox* m_enablePex;
    };
}
