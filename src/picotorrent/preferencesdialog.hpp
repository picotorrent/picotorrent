#pragma once

#include <QDialog>

class QWidget;

namespace pt
{
    class PreferencesDialog : public QDialog
    {
    public:
        PreferencesDialog(QWidget* parent);
    };
}
