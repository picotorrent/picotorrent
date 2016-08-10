using System.Collections.Generic;
using System.Linq;

namespace PicoTorrent.Plugins.AutoAdd.Data
{
    public sealed class HistoryRepository : IHistoryRepository
    {
        private readonly HashSet<string> _set;

        public HistoryRepository()
        {
            _set = new HashSet<string>();
        }

        public void Clear()
        {
            _set.Clear();
        }

        public bool Contains(string hash)
        {
            return _set.Contains(hash);
        }

        public IEnumerable<string> GetAll()
        {
            return _set.ToList();
        }

        public void Insert(string hash)
        {
            if (!_set.Contains(hash))
            {
                _set.Add(hash);
            }
        }
    }
}
