#pragma once

#include <QDialog>

#include <memory>

class QDialogButtonBox;
class QModelIndex;
class QStackedWidget;
class QWidget;

class SectionListView;

namespace Ui
{
    class PreferencesDialog;
}

namespace pt
{
    class Configuration;
    class ConnectionPreferencesPage;
    class DownloadsPreferencesPage;
    class Environment;
    class GeneralPreferencesPage;
    class ProxyPreferencesPage;

    class PreferencesDialog : public QDialog
    {
    public:
        PreferencesDialog(QWidget* parent, std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env);
        virtual ~PreferencesDialog();

        void load();

    private:
        void onOk();
        void onSectionActivated(QModelIndex const& index);

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;

        ::Ui::PreferencesDialog* m_ui;

        GeneralPreferencesPage* m_general;
        DownloadsPreferencesPage* m_downloads;
        ConnectionPreferencesPage* m_connection;
        ProxyPreferencesPage* m_proxy;
    };
}
