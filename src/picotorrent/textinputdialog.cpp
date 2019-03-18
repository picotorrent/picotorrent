#include "textinputdialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

using pt::TextInputDialog;

TextInputDialog::TextInputDialog(QWidget* parent, QString const& description, bool multiline)
    : QDialog(parent),
    m_description(description),
    m_multiline(multiline)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::Dialog;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    m_input = new QLineEdit(this);
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto layout = new QVBoxLayout();
    layout->addWidget(new QLabel(description));
    layout->addWidget(m_input);
    layout->addWidget(m_buttons);

    QObject::connect(m_buttons, &QDialogButtonBox::accepted,
                     this,      &TextInputDialog::accept);

    QObject::connect(m_buttons, &QDialogButtonBox::rejected,
                     this,      &TextInputDialog::reject);

    this->setLayout(layout);
    this->setWindowFlags(flags);
}

QString TextInputDialog::text()
{
    return m_input->text();
}
