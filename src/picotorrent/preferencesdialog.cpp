#include "preferencesdialog.hpp"
#include "ui_preferencesdialog.h"

#include <QAbstractListModel>
#include <QDialogButtonBox>
#include <QGroupbox>
#include <QHBoxLayout>
#include <QListView>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <vector>

#include "core/configuration.hpp"
#include "core/environment.hpp"
#include "connectionpreferencespage.hpp"
#include "downloadssectionwidget.hpp"
#include "generalsectionwidget.hpp"
#include "proxysectionwidget.hpp"
#include "translator.hpp"

using pt::PreferencesDialog;

class SectionListModel : public QAbstractListModel
{
public:
    void addItem(QString const& text)
    {
        int nextIndex = static_cast<int>(m_items.size());

        beginInsertRows(QModelIndex(), nextIndex, nextIndex);
        m_items.push_back(text);
        endInsertRows();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return static_cast<int>(m_items.size());
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        switch (role) {
        case Qt::DisplayRole:
            return m_items.at(index.row());
        }

        return QVariant();
    }

private:
    std::vector<QString> m_items;
};

PreferencesDialog::PreferencesDialog(QWidget* parent, std::shared_ptr<pt::Configuration> cfg, std::shared_ptr<pt::Environment> env)
    : QDialog(parent),
    m_cfg(cfg),
    m_env(env),
    m_ui(new Ui::PreferencesDialog())
{
    m_ui->setupUi(this);

    auto model = new SectionListModel();
    model->addItem(i18n("general"));
    model->addItem(i18n("downloads"));
    model->addItem(i18n("connection"));
    model->addItem(i18n("proxy"));

    m_ui->sectionsList->setModel(model);

    m_general = new GeneralSectionWidget();
    m_downloads = new DownloadsSectionWidget();
    m_connection = new ConnectionPreferencesPage();
    m_proxy = new ProxySectionWidget();

    m_ui->sections->addWidget(m_general);
    m_ui->sections->addWidget(m_downloads);
    m_ui->sections->addWidget(m_connection);
    m_ui->sections->addWidget(m_proxy);

    connect(m_ui->sectionsList, &QListView::clicked,         this, &PreferencesDialog::onSectionActivated);
    connect(m_ui->buttons,  &QDialogButtonBox::accepted, this, &PreferencesDialog::onOk);
    connect(m_ui->buttons,  &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    this->setWindowFlags(flags);
    this->setWindowTitle(i18n("preferences"));
}

void PreferencesDialog::load()
{
    m_general->loadConfig(m_cfg, m_env);
    m_downloads->loadConfig(m_cfg);
    m_connection->loadConfig(m_cfg);
    m_proxy->loadConfig(m_cfg);
}

void PreferencesDialog::onOk()
{
    bool requiresRestart = false;

    m_general->saveConfig(m_cfg, m_env, &requiresRestart);
    m_downloads->saveConfig(m_cfg);
    m_connection->saveConfig(m_cfg);
    m_proxy->saveConfig(m_cfg);

    if (requiresRestart)
    {
        QMessageBox mbox(this);
        mbox.addButton(QMessageBox::Ok);
        mbox.setIcon(QMessageBox::Information);
        mbox.setText(i18n("prompt_restart"));
        mbox.setWindowTitle(i18n("prompt_restart_title"));
        mbox.exec();
    }

    this->done(QDialog::Accepted);
}

void PreferencesDialog::onSectionActivated(QModelIndex const& index)
{
    m_ui->sections->setCurrentIndex(index.row());
}
