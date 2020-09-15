using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using System;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class InstallApplyingViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private int _cacheProgress;
        private int _executeProgress;
        private string _message;
        private int _progress;
        private int _progressPhases;

        public InstallApplyingViewModel(BootstrapperApplication bootstrapper)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
            _bootstrapper.ApplyPhaseCount += (sender, args) => _progressPhases = args.PhaseCount;
            _bootstrapper.CacheAcquireProgress += OnCacheAcquireProgress;
            _bootstrapper.CacheComplete += OnCacheComplete;
            _bootstrapper.ExecuteMsiMessage += OnExecuteMsiMessage;
            _bootstrapper.ExecuteProgress += OnExecuteProgress;
        }

        public string Message
        {
            get { return _message; }
            set { _message = value; OnPropertyChanged(nameof(Message)); }
        }

        public int Progress
        {
            get { return _progress; }
            set { _progress = value; OnPropertyChanged(nameof(Progress)); }
        }

        private void OnCacheAcquireProgress(object sender, CacheAcquireProgressEventArgs e)
        {
            lock (this)
            {
                _cacheProgress = e.OverallPercentage;
                Progress = (_cacheProgress + _executeProgress) / _progressPhases;
            }
        }

        private void OnCacheComplete(object sender, CacheCompleteEventArgs e)
        {
            lock (this)
            {
                _cacheProgress = 100;
                Progress = (_cacheProgress + _executeProgress) / _progressPhases;
            }
        }

        private void OnExecuteMsiMessage(object sender, ExecuteMsiMessageEventArgs e)
        {
            lock (this)
            {
                Message = e.Message;
            }
        }

        private void OnExecuteProgress(object sender, ExecuteProgressEventArgs e)
        {
            lock (this)
            {
                _executeProgress = e.OverallPercentage;
                Progress = (_cacheProgress + _executeProgress) / _progressPhases;

                if (_bootstrapper.Command.Display == Display.Embedded)
                {
                    _bootstrapper.Engine.SendEmbeddedProgress(e.ProgressPercentage, Progress);
                }
            }
        }
    }
}
