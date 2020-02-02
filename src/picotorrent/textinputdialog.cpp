#include "textinputdialog.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

using pt::TextInputDialog;

TextInputDialog::TextInputDialog(QWidget* parent, QString const& description, bool multiline)
    : QDialog(parent),
    m_description(description),
    m_multiline(multiline),
    m_singleLineInput(nullptr),
    m_multiLineInput(nullptr)
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::Dialog;
    flags &= ~Qt::WindowContextHelpButtonHint;
    flags &= ~Qt::WindowSystemMenuHint;

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto layout = new QVBoxLayout();

    if (description.size() > 0)
    {
        layout->addWidget(new QLabel(description, this));
    }

    if (m_multiline)
    {
        m_multiLineInput = new QPlainTextEdit(this);
        m_multiLineInput->setLineWrapMode(QPlainTextEdit::NoWrap);
        m_multiLineInput->setWordWrapMode(QTextOption::NoWrap);
        layout->addWidget(m_multiLineInput);
    }
    else
    {
        m_singleLineInput = new QLineEdit(this);
        layout->addWidget(m_singleLineInput);
    }

    layout->addWidget(m_buttons);

    QObject::connect(m_buttons, &QDialogButtonBox::accepted,
                     this,      &TextInputDialog::accept);

    QObject::connect(m_buttons, &QDialogButtonBox::rejected,
                     this,      &TextInputDialog::reject);

    this->setLayout(layout);
    this->setWindowFlags(flags);
    this->setFocusToText();
}

void TextInputDialog::setFocusToText()
{
    if (m_multiline)
    {
        m_multiLineInput->setFocus();
    }
    else
    {
        m_singleLineInput->setFocus();
    }
}

QString TextInputDialog::text()
{
    if (m_multiline)
    {
        return m_multiLineInput->toPlainText();
    }

    return m_singleLineInput->text();
}

void TextInputDialog::setText(QString text)
{
    if (m_multiline)
    {
        m_multiLineInput->setPlainText(text);
    }
    else
    {
        m_singleLineInput->setText(text);
    }
}

