using PicoTorrent.UI;
using System;

namespace PicoTorrent
{
    public interface IPlugin
    {
        string Name { get; }

        Version Version { get; }

        IWindow GetWindow();

        void Load();

        void Unload();
    }
}
