#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <msclr/marshal_cppstd.h>

namespace PicoTorrent
{
namespace UI
{
    class TaskDialogWrapper;

    ref class TaskDialog : public ITaskDialog
    {
    public:
        TaskDialog(HWND hwndOwner);
        ~TaskDialog();

        // Events
        virtual event System::EventHandler<PicoTorrent::UI::TaskDialogButtonClickedEventArgs^>^ ButtonClicked;
        virtual event System::EventHandler^ Created;
        virtual event System::EventHandler^ Destroyed;
        virtual event System::EventHandler<PicoTorrent::UI::TaskDialogVerificationClickedEventArgs^>^ VerificationClicked;

        virtual property TaskDialogBehavior Behavior { TaskDialogBehavior get(); void set(TaskDialogBehavior); };
        virtual property TaskDialogButton CommonButtons { TaskDialogButton get(); void set(TaskDialogButton); };
        virtual property System::String^ Content { System::String^ get(); void set(System::String^); };
        virtual property TaskDialogIcon MainIcon { TaskDialogIcon get(); void set(TaskDialogIcon); };
        virtual property System::String^ MainInstruction { System::String^ get(); void set(System::String^); };
        virtual property System::String^ Title { System::String^ get(); void set(System::String^); };
        virtual property System::String^ VerificationText { System::String^ get(); void set(System::String^); };

        virtual void AddButton(int buttonId, System::String^ buttonText);
        virtual void Show();

        // Event raisers
        void RaiseButtonClicked(PicoTorrent::UI::TaskDialogButtonClickedEventArgs^);
        void RaiseCreated(System::EventArgs^);
        void RaiseDestroyed(System::EventArgs^);
        void RaiseVerificationClicked(PicoTorrent::UI::TaskDialogVerificationClickedEventArgs^);

    private:
        TASKDIALOGCONFIG* _dlg;
        TaskDialogWrapper* _wrapper;
        msclr::interop::marshal_context^ _marshal;
    };
}
}
