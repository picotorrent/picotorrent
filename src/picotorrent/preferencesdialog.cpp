#include "preferencesdialog.hpp"

#include <QAbstractListModel>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedLayout>
#include <QVBoxLayout>

using pt::PreferencesDialog;

class SectionListModel : public QAbstractListModel
{
public:
    SectionListModel()
    {
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        switch (role) {
        case Qt::DisplayRole:
            return "Hello";
        }

        return QVariant();
    }
};

class SectionListView : public QListView
{
public:
    SectionListView()
    {
        this->setModel(new SectionListModel());
    }
};

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::CustomizeWindowHint;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    auto horiz = new QHBoxLayout();
    horiz->addWidget(new SectionListView());
    horiz->addWidget(new QPushButton("Hello"));
    horiz->setStretch(1, 2);

    this->setLayout(horiz);
    this->setWindowFlags(flags);
    this->setWindowTitle("Preferences");
}
