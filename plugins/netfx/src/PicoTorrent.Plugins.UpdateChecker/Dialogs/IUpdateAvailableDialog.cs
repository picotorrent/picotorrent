using System;

namespace PicoTorrent.Plugins.UpdateChecker.Dialogs
{
    public interface IUpdateAvailableDialog
    {
        void Show(Version version, string releaseUrl);
    }
}
