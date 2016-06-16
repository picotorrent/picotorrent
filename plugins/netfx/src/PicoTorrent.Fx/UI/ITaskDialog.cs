using System;

namespace PicoTorrent.UI
{
    public interface ITaskDialog : IDisposable
    {
        // Events
        event EventHandler<TaskDialogButtonClickedEventArgs> ButtonClicked;
        event EventHandler Created;
        event EventHandler Destroyed;
        event EventHandler<TaskDialogVerificationClickedEventArgs> VerificationClicked;

        TaskDialogBehavior Behavior { get; set; }

        TaskDialogButton CommonButtons { get; set; }

        string Content { get; set; }

        TaskDialogIcon MainIcon { get; set; }

        string MainInstruction { get; set; }

        string Title { get; set; }

        string VerificationText { get; set; }

        void AddButton(int buttonId, string buttonText);

        void Show();
    }
}
