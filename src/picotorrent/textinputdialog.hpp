#pragma once

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;
class QPlainTextEdit;

namespace pt
{
    class TextInputDialog : public QDialog
    {
    public:
        TextInputDialog(QWidget* parent, QString const& description = "", bool multiline = false);
        void setFocusToText();
        QString text();
        void setText(QString text);

    private:
        QDialogButtonBox* m_buttons;
        QLineEdit* m_singleLineInput;
        QPlainTextEdit* m_multiLineInput;
        QString m_description;
        bool m_multiline;
    };
}
