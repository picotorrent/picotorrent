using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using System;
using System.ComponentModel;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class ProgressViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private readonly MainViewModel _mainModel;

        private int _cacheProgress;
        private int _executeProgress;
        private string _message;
        private int _progress;
        private int _progressPhases;

        public ProgressViewModel(BootstrapperApplication bootstrapper, MainViewModel mainModel)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
            _mainModel = mainModel ?? throw new ArgumentNullException(nameof(mainModel));

            _bootstrapper.ApplyPhaseCount += (sender, args) => _progressPhases = args.PhaseCount;
            _bootstrapper.CacheAcquireProgress += OnCacheAcquireProgress;
            _bootstrapper.CacheComplete += OnCacheComplete;
            _bootstrapper.ExecuteMsiMessage += OnExecuteMsiMessage;
            _bootstrapper.ExecuteProgress += OnExecuteProgress;
            _bootstrapper.Progress += OnProgress;

            _mainModel.PropertyChanged += OnMainModelPropertyChanged;
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

        public bool ProgressEnabled
        {
            get { return _mainModel.InstallState == InstallationState.Applying; }
        }

        private void OnCacheAcquireProgress(object sender, CacheAcquireProgressEventArgs e)
        {
            lock (this)
            {
                _cacheProgress = e.OverallPercentage;
                Progress = (_cacheProgress + _executeProgress) / _progressPhases;

                e.Result = _mainModel.Canceled
                    ? Result.Cancel
                    : Result.Ok;
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
                if (e.MessageType == InstallMessage.ActionStart)
                {
                    Message = e.Message;
                }

                e.Result = _mainModel.Canceled
                    ? Result.Cancel
                    : Result.Ok;
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

                e.Result = _mainModel.Canceled
                    ? Result.Cancel
                    : Result.Ok;
            }
        }

        private void OnProgress(object sender, ProgressEventArgs e)
        {
            lock (this)
            {
                e.Result = _mainModel.Canceled
                    ? Result.Cancel
                    : Result.Ok;
            }
        }

        private void OnMainModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "InstallState")
            {
                OnPropertyChanged("ProgressEnabled");
            }
        }
    }
}
