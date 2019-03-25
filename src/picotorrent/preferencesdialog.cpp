#include "preferencesdialog.hpp"

#include <QAbstractListModel>
#include <QDialogButtonBox>
#include <QGroupbox>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <vector>

#include "core/configuration.hpp"
#include "connectionsectionwidget.hpp"
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

class SectionListView : public QListView
{
public:
    using QListView::QListView;

    QSize sizeHint() const override
    {
        return QSize(
            sizeHintForColumn(0),
            QListView::sizeHint().height());
    }
};

PreferencesDialog::PreferencesDialog(QWidget* parent, std::shared_ptr<pt::Configuration> cfg)
    : QDialog(parent),
    m_cfg(cfg)
{
    createUi();

    auto model = new SectionListModel();
    model->addItem(i18n("general"));
    model->addItem(i18n("downloads"));
    model->addItem(i18n("connection"));
    model->addItem(i18n("proxy"));

    m_sections->setModel(model);

    connect(m_sections, &QListView::clicked,         this, &PreferencesDialog::onSectionActivated);
    connect(m_buttons,  &QDialogButtonBox::accepted, this, &PreferencesDialog::onOk);
    connect(m_buttons,  &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    this->setWindowFlags(flags);
    this->setWindowTitle(i18n("preferences"));
}

void PreferencesDialog::load()
{
    m_general->loadConfig(m_cfg);
    m_downloads->loadConfig(m_cfg);
    m_connection->loadConfig(m_cfg);
    m_proxy->loadConfig(m_cfg);
}

void PreferencesDialog::createUi()
{
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    m_stacked = new QStackedWidget(this);
    m_sections = new SectionListView();
    m_sections->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    m_general = new GeneralSectionWidget();
    m_downloads = new DownloadsSectionWidget();
    m_connection = new ConnectionSectionWidget();
    m_proxy = new ProxySectionWidget();

    m_stacked->addWidget(m_general);
    m_stacked->addWidget(m_downloads);
    m_stacked->addWidget(m_connection);
    m_stacked->addWidget(m_proxy);

    auto horiz = new QHBoxLayout();
    horiz->addWidget(m_sections);
    horiz->addWidget(m_stacked);

    auto vert = new QVBoxLayout(this);
    vert->addLayout(horiz);
    vert->addWidget(m_buttons);
    vert->setMargin(5);

    this->setLayout(vert);
}

void PreferencesDialog::onOk()
{
    m_general->saveConfig(m_cfg);
    m_downloads->saveConfig(m_cfg);
    m_connection->saveConfig(m_cfg);
    m_proxy->saveConfig(m_cfg);

    this->done(QDialog::Accepted);
}

void PreferencesDialog::onSectionActivated(QModelIndex const& index)
{
    m_stacked->setCurrentIndex(index.row());
}
