using System;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;

using PicoTorrent.Core;
using PicoTorrent.Logging;
using PicoTorrent.Plugins.AutoAdd.Models;
using PicoTorrent.Plugins.AutoAdd.Data;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace PicoTorrent.Plugins.AutoAdd
{
    public sealed class FolderWatcher : IFolderWatcher
    {
        private readonly ILogger _logger;
        private readonly IAutoAddConfig _config;
        private readonly ISession _session;
        private readonly IHistoryRepository _history;

        private readonly CancellationTokenSource _cancelTokenSource;
        private readonly SHA1Managed _hasher;
        private readonly Task _pollTask;

        public FolderWatcher(
            ILogger logger,
            IAutoAddConfig config,
            ISession session,
            IHistoryRepository history)
        {
            if (logger == null) throw new ArgumentNullException(nameof(logger));
            if (config == null) throw new ArgumentNullException(nameof(config));
            if (session == null) throw new ArgumentNullException(nameof(session));
            if (history == null) throw new ArgumentNullException(nameof(history));
            _logger = logger;
            _config = config;
            _session = session;
            _history = history;

            _cancelTokenSource = new CancellationTokenSource();
            _hasher = new SHA1Managed();
            _pollTask = new Task(Poll, _cancelTokenSource.Token);
        }

        public void Start()
        {
            _pollTask.Start();
        }

        public void Stop()
        {
            _logger.Debug("Waiting for poll task to cancel.");

            _cancelTokenSource.Cancel();
            _pollTask.Wait();

            _logger.Debug("Poll task canceled.");
        }

        private async void Poll()
        {
            try
            {
                await PollAsync(_cancelTokenSource.Token);
            }
            catch (OperationCanceledException)
            {
            }
            catch (Exception exception)
            {
                _logger.Error("Unhandled exception occured when running poll task.", exception);
            }
        }

        private async Task PollAsync(CancellationToken cancelToken)
        {
            while (true)
            {
                cancelToken.ThrowIfCancellationRequested();

                var folders = _config.Folders;

                try
                {
                    foreach (var folder in folders)
                    {
                        CheckFolder(folder);
                    }
                }
                catch (Exception exception)
                {
                    _logger.Error("An error occured when checking folders.", exception);
                }

                await Task.Delay(_config.PollInterval * 1000, cancelToken);
            }
        }

        private void CheckFolder(Folder folder)
        {
            if (!Directory.Exists(folder.Path))
            {
                return;
            }

            var files = Directory.GetFiles(folder.Path, "*", SearchOption.TopDirectoryOnly);

            if (files == null || !files.Any())
            {
                return;
            }

            foreach (var file in files)
            {
                var fileName = Path.GetFileName(file);

                if (!Regex.IsMatch(fileName, folder.Pattern))
                {
                    continue;
                }

                var hash = ComputeHistoryHash(file);

                if (_history.Contains(hash))
                {
                    return;
                }

                _session.AddTorrent(file);
                _history.Insert(hash);

                if (folder.RemoveFile)
                {
                    File.Delete(file);
                }
            }
        }

        private string ComputeHistoryHash(string input)
        {
            var data = Encoding.UTF8.GetBytes(input);
            var sb = new StringBuilder();

            foreach (var b in _hasher.ComputeHash(data))
            {
                sb.AppendFormat("{0:x2}", b);
            }

            return sb.ToString();
        }
    }
}
