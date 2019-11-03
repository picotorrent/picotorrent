#pragma once

#include <QDialog>

#include <memory>

class QDialogButtonBox;
class QModelIndex;
class QStackedWidget;
class QWidget;

class SectionListView;

namespace pt
{
    class Configuration;
    class ConnectionSectionWidget;
    class DownloadsSectionWidget;
    class Environment;
    class GeneralSectionWidget;
    class ProxySectionWidget;

    class PreferencesDialog : public QDialog
    {
    public:
        PreferencesDialog(QWidget* parent, std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env);
        void load();

    private:
        void createUi();
        void onOk();
        void onSectionActivated(QModelIndex const& index);

        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;

        QStackedWidget* m_stacked;
        QDialogButtonBox* m_buttons;
        SectionListView* m_sections;

        GeneralSectionWidget* m_general;
        DownloadsSectionWidget* m_downloads;
        ConnectionSectionWidget* m_connection;
        ProxySectionWidget* m_proxy;
    };
}
