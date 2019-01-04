#pragma once

#include <QWidget>

#include <memory>

class QCheckBox;
class QComboBox;
class QLineEdit;

namespace pt
{
    class Configuration;

    class ProxySectionWidget : public QWidget
    {
    public:
        ProxySectionWidget();
        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void createUi();
        void onProxyTypeChanged(int index);

        QComboBox* m_proxyType;
        QLineEdit* m_proxyHost;
        QLineEdit* m_proxyPort;
        QLineEdit* m_proxyUsername;
        QLineEdit* m_proxyPassword;
        QCheckBox* m_forceProxy;
        QCheckBox* m_proxyHostnames;
        QCheckBox* m_proxyPeerConnections;
        QCheckBox* m_proxyTrackerConnections;
    };
}
