using System;

namespace PicoTorrent.UI
{
    public sealed class TaskDialogButtonClickedEventArgs : EventArgs
    {
        public TaskDialogButtonClickedEventArgs(int buttonId)
        {
            ButtonId = buttonId;
        }

        public int ButtonId { get; }
    }
}
