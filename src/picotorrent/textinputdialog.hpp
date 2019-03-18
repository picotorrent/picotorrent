#pragma once

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;

namespace pt
{
    class TextInputDialog : public QDialog
    {
    public:
        TextInputDialog(QWidget* parent, QString const& description = "", bool multiline = false);
        QString text();

    private:
        QDialogButtonBox* m_buttons;
        QLineEdit* m_input;
        QString m_description;
        bool m_multiline;
    };
}
