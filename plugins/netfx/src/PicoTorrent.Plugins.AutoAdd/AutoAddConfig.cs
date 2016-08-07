using System;
using System.Collections.Generic;
using System.Linq;
using PicoTorrent.Plugins.AutoAdd.Models;

namespace PicoTorrent.Plugins.AutoAdd
{
    public sealed class AutoAddConfig : ConfigurationPart, IAutoAddConfig
    {
        public AutoAddConfig(IConfiguration config)
            : base(config, "autoadd")
        {
        }

        public IEnumerable<Folder> Folders
        {
            get { return Get("folders", new List<IDictionary<string, object>>()).Select(MapFolder); }
            set { Set("folders", value.Select(MapFolder)); }
        }

        public int PollInterval
        {
            get { return Get("poll_interval", 3); }
            set { Set("poll_interval", value); }
        }

        private Folder MapFolder(IDictionary<string, object> obj)
        {
            return new Folder
            {
                Path = obj["path"].ToString(),
                Pattern = obj["pattern"].ToString(),
                RemoveFile = (bool)obj["remove_file"]
            };
        }

        private object MapFolder(Folder folder)
        {
            return new
            {
                path = folder.Path,
                pattern = folder.Pattern,
                remove_file = folder.RemoveFile
            };
        }
    }
}
