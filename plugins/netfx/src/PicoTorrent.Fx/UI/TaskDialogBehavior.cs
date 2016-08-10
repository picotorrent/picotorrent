using System;

namespace PicoTorrent.UI
{
    [Flags]
    public enum TaskDialogBehavior
    {
        EnableHyperlinks = 0x0001,
//        TDF_USE_HICON_MAIN = 0x0002,
//        TDF_USE_HICON_FOOTER = 0x0004,
        AllowDialogCancellation = 0x0008,
        UseCommandLinks = 0x0010,
        UseCommandLinksNoIcon = 0x0020,
        ExpandFooterArea = 0x0040,
        ExpandedByDefault = 0x0080,
        VerificationFlagChecked = 0x0100,
        ShowProgressBar = 0x0200,
        ShowMarqueeProgressBar = 0x0400,
        CallbackTimer = 0x0800,
        PositionRelativeToWindow = 0x1000,
        RtlLayout = 0x2000,
        NoDefaultRadioButton = 0x4000,
        CanBeMinimized = 0x8000,
        NoSetForeground = 0x00010000,
        SizeToContent = 0x01000000
    }
}
