using System;

namespace PicoTorrent.UI
{
    [Flags]
    public enum TaskDialogButton
    {
        Ok = 0x0001,
        Yes = 0x0002,
        No = 0x0004,
        Cancel = 0x0008,
        Retry = 0x0010,
        Close = 0x0020
    }
}
