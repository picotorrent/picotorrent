using System;

namespace PicoTorrent.UI
{
    public interface IMainWindow
    {
        event EventHandler<CommandEventArgs> Command;

        ITaskDialog CreateTaskDialog();

        IMainMenu MainMenu { get; }

        void ShowMessageBox(string title, string message);
    }
}
