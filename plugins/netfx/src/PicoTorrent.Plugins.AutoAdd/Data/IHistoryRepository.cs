using System.Collections.Generic;

namespace PicoTorrent.Plugins.AutoAdd.Data
{
    public interface IHistoryRepository
    {
        void Clear();
        bool Contains(string hash);
        IEnumerable<string> GetAll();
        void Insert(string hash);
    }
}
