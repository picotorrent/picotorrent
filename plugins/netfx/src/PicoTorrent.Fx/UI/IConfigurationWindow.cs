using System;

namespace PicoTorrent.UI
{
    public interface IConfigurationWindow
    {
        event EventHandler Dirty;

        IntPtr GetHandle();

        void Load();

        void Save();
    }
}
