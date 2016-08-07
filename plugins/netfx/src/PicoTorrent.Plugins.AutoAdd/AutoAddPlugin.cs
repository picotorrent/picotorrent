using System;
using System.IO;

using PicoTorrent.UI;
using PicoTorrent.Plugins.AutoAdd.Data;
using System.Text;

namespace PicoTorrent.Plugins.AutoAdd
{
    public class AutoAddPlugin : Plugin
    {
        private readonly IEnvironment _environment;
        private readonly IConfigurationWindow _configWindow;
        private readonly IFolderWatcher _folderWatcher;
        private readonly IHistoryRepository _history;

        public AutoAddPlugin(
            IEnvironment environment,
            IConfigurationWindow configWindow,
            IFolderWatcher folderWatcher,
            IHistoryRepository history)
        {
            if (environment == null) throw new ArgumentNullException(nameof(environment));
            if (configWindow == null) throw new ArgumentNullException(nameof(configWindow));
            if (folderWatcher == null) throw new ArgumentNullException(nameof(folderWatcher));
            if (history == null) throw new ArgumentNullException(nameof(history));
            _environment = environment;
            _configWindow = configWindow;
            _folderWatcher = folderWatcher;
            _history = history;
        }

        public override IConfigurationWindow GetConfigurationWindow()
        {
            return _configWindow;
        }

        public override void Load()
        {
            LoadHistory();
           _folderWatcher.Start();
        }

        public override void Unload()
        {
            _folderWatcher.Stop();
            SaveHistory();
        }

        private void LoadHistory()
        {
            var dat = Path.Combine(_environment.DataPath, "autoadd", "history.dat");

            if (!File.Exists(dat))
            {
                return;
            }

            using (var reader = new StreamReader(dat, Encoding.UTF8))
            {
                while (!reader.EndOfStream)
                {
                    _history.Insert(reader.ReadLine());
                }
            }
        }

        private void SaveHistory()
        {
            var path = Path.Combine(_environment.DataPath, "autoadd");
            var dat = Path.Combine(path, "history.dat");

            if (!Directory.Exists(path))
            {
                Directory.CreateDirectory(path);
            }

            using (var writer = new StreamWriter(dat, false, Encoding.UTF8))
            {
                foreach(var hash in _history.GetAll())
                {
                    writer.WriteLine(hash);
                }
            }
        }
    }
}
