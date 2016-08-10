using PicoTorrent.UI;
using System;

namespace PicoTorrent
{
    public interface IPlugin
    {
        string Name { get; }

        Version Version { get; }

        IConfigurationWindow GetConfigurationWindow();

        void Load();

        void Unload();
    }
}
