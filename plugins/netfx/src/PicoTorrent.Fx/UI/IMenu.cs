using System.Collections.Generic;

namespace PicoTorrent.UI
{
    public interface IMenu
    {
        void Insert(int id, string text);

        void InsertSeparator();

        IReadOnlyCollection<IMenuItem> Items { get; }
    }
}
