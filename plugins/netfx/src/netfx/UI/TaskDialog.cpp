#include "TaskDialog.h"

#include <msclr/marshal_cppstd.h>

using PicoTorrent::UI::TaskDialog;
using PicoTorrent::UI::TaskDialogButtonClickedEventArgs;
using PicoTorrent::UI::TaskDialogVerificationClickedEventArgs;
using PicoTorrent::UI::TaskDialogWrapper;

class TaskDialogWrapper
{
public:
    TaskDialogWrapper(gcroot<PicoTorrent::UI::TaskDialog^> dialog)
        : _dialog(dialog)
    {
    }

    gcroot<PicoTorrent::UI::TaskDialog^> get() { return _dialog; }

private:
    gcroot<PicoTorrent::UI::TaskDialog^> _dialog;
};

HRESULT CALLBACK TaskDialogCallback(HWND hWnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
{
    TaskDialogWrapper* wrapper = reinterpret_cast<TaskDialogWrapper*>(dwRefData);

    switch (uNotification)
    {
    case TDN_BUTTON_CLICKED:
    {
        auto args = gcnew TaskDialogButtonClickedEventArgs((int)wParam);
        wrapper->get()->RaiseButtonClicked(args);
        break;
    }
    case TDN_CREATED:
    {
        wrapper->get()->RaiseCreated(System::EventArgs::Empty);
        break;
    }
    case TDN_DESTROYED:
    {
        wrapper->get()->RaiseDestroyed(System::EventArgs::Empty);
        break;
    }
    case TDN_VERIFICATION_CLICKED:
    {
        auto args = gcnew TaskDialogVerificationClickedEventArgs((BOOL)wParam == TRUE);
        wrapper->get()->RaiseVerificationClicked(args);
        break;
    }
    }

    return S_OK;
}

TaskDialog::TaskDialog(HWND hwndOwner)
    : _dlg(new TASKDIALOGCONFIG()),
    _wrapper(new TaskDialogWrapper(this))
{
    _dlg->cbSize = sizeof(TASKDIALOGCONFIG);
    _dlg->hInstance = GetModuleHandle(NULL);
    _dlg->hwndParent = hwndOwner;
    _dlg->lpCallbackData = (LONG_PTR)_wrapper;
    _dlg->pfCallback = &TaskDialogCallback;

    _marshal = gcnew msclr::interop::marshal_context();
}

TaskDialog::~TaskDialog()
{
    delete _dlg->pButtons;
    delete _dlg;
    delete _wrapper;
}

PicoTorrent::UI::TaskDialogBehavior TaskDialog::Behavior::get()
{
    return static_cast<TaskDialogBehavior>(_dlg->dwFlags);
}

void TaskDialog::Behavior::set(PicoTorrent::UI::TaskDialogBehavior value)
{
    _dlg->dwFlags = static_cast<TASKDIALOG_FLAGS>(value);
}

PicoTorrent::UI::TaskDialogButton TaskDialog::CommonButtons::get()
{
    return static_cast<TaskDialogButton>(_dlg->dwCommonButtons);
}

void TaskDialog::CommonButtons::set(PicoTorrent::UI::TaskDialogButton value)
{
    _dlg->dwCommonButtons = static_cast<DWORD>(value);
}

System::String^ TaskDialog::Content::get()
{
    return msclr::interop::marshal_as<System::String^>(_dlg->pszContent);
}

void TaskDialog::Content::set(System::String^ value)
{
    _dlg->pszContent = _marshal->marshal_as<LPCWSTR>(value);
}

PicoTorrent::UI::TaskDialogIcon TaskDialog::MainIcon::get()
{
    return UI::TaskDialogIcon::Error;
    //return static_cast<TaskDialogIcon>(_dlg->pszMainIcon);
}

void TaskDialog::MainIcon::set(PicoTorrent::UI::TaskDialogIcon value)
{
    _dlg->pszMainIcon = MAKEINTRESOURCE(value);
}

System::String^ TaskDialog::MainInstruction::get()
{
    return msclr::interop::marshal_as<System::String^>(_dlg->pszMainInstruction);
}

void TaskDialog::MainInstruction::set(System::String^ value)
{
    _dlg->pszMainInstruction = _marshal->marshal_as<LPCWSTR>(value);
}

System::String^ TaskDialog::Title::get()
{
    return msclr::interop::marshal_as<System::String^>(_dlg->pszWindowTitle);
}

void TaskDialog::Title::set(System::String^ value)
{
    _dlg->pszWindowTitle = _marshal->marshal_as<LPCWSTR>(value);
}

System::String^ TaskDialog::VerificationText::get()
{
    return msclr::interop::marshal_as<System::String^>(_dlg->pszVerificationText);
}

void TaskDialog::VerificationText::set(System::String^ value)
{
    _dlg->pszVerificationText = _marshal->marshal_as<LPCWSTR>(value);
}

void TaskDialog::AddButton(int buttonId, System::String^ buttonText)
{
    int currentButtons = _dlg->cButtons;

    TASKDIALOG_BUTTON* buttonArray = new TASKDIALOG_BUTTON[currentButtons + 1];
    memcpy(buttonArray, _dlg->pButtons, currentButtons * sizeof(TASKDIALOG_BUTTON));

    // Add our new button to the end of the new array
    buttonArray[currentButtons] = TASKDIALOG_BUTTON{ buttonId, _marshal->marshal_as<LPCWSTR>(buttonText) };

    delete[] _dlg->pButtons;

    _dlg->cButtons = currentButtons + 1;
    _dlg->pButtons = buttonArray;    
}

void TaskDialog::Show()
{
    int nButton;
    int nRadioButton;
    BOOL fVerificationFlagChecked;

    HRESULT hr = TaskDialogIndirect(_dlg, &nButton, &nRadioButton, &fVerificationFlagChecked);
}

void TaskDialog::RaiseButtonClicked(TaskDialogButtonClickedEventArgs^ args)
{
    ButtonClicked(this, args);
}

void TaskDialog::RaiseCreated(System::EventArgs^ args)
{
    Created(this, args);
}

void TaskDialog::RaiseDestroyed(System::EventArgs^ args)
{
    Destroyed(this, args);
}

void TaskDialog::RaiseVerificationClicked(TaskDialogVerificationClickedEventArgs^ args)
{
    VerificationClicked(this, args);
}
