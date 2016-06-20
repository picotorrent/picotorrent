using System;

namespace PicoTorrent.UI
{
    public interface IConfigurationWindow
    {
        IntPtr GetHandle();

        void Load();

        void Save();
    }
}
