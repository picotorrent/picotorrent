#pragma once

#include <QWidget>

#include <memory>

namespace pt
{
    namespace Ui
    {
        class ProxyPreferencesPage;
    }

    class Configuration;

    class ProxyPreferencesPage : public QWidget
    {
    public:
        ProxyPreferencesPage(QWidget* parent);
        virtual ~ProxyPreferencesPage();

        void loadConfig(std::shared_ptr<Configuration> cfg);
        void saveConfig(std::shared_ptr<Configuration> cfg);

    private:
        void onProxyTypeChanged(int index);

        Ui::ProxyPreferencesPage* m_ui;
    };
}
