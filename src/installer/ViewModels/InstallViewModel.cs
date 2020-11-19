using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using PicoTorrentBootstrapper.Views;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Input;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class InstallViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private readonly MainViewModel _mainModel;

        private bool _isUpgrade;
        private ICommand _installCommand;
        private ICommand _launchCommand;

        public InstallViewModel(BootstrapperApplication bootstrapper, MainViewModel mainModel)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
            _bootstrapper.ResolveSource += OnResolveSource;

            _mainModel = mainModel ?? throw new ArgumentNullException(nameof(mainModel));
            _mainModel.PropertyChanged += OnMainModelPropertyChanged;

            InstallFailedModel = new InstallFailedViewModel(bootstrapper);
            InstallWaitingModel = new InstallWaitingViewModel(bootstrapper);
            ProgressModel = new ProgressViewModel(bootstrapper, mainModel);
            IsUpgrade = false;
        }

        public InstallFailedViewModel InstallFailedModel { get; private set; }

        public InstallWaitingViewModel InstallWaitingModel { get; private set; }

        public ProgressViewModel ProgressModel { get; private set; }

        public ICommand CancelCommand => _mainModel.CancelCommand;

        public bool CancelEnabled => _mainModel.CancelCommand.CanExecute(this);

        public ICommand CloseCommand => _mainModel.CloseCommand;

        public bool CloseEnabled => _mainModel.InstallState != InstallationState.Applying;

        public ICommand InstallCommand
        {
            get
            {
                if (_installCommand == null)
                {
                    _installCommand = new RelayCommand(
                        param => { _mainModel.Plan(LaunchAction.Install); },
                        param => _mainModel.DetectState == DetectionState.Absent && _mainModel.InstallState == InstallationState.Waiting);
                }

                return _installCommand;
            }
        }

        public ICommand LaunchCommand
        {
            get
            {
                if (_launchCommand == null)
                {
                    _launchCommand = new RelayCommand(
                        param => LaunchApplication(),
                        param => _mainModel.InstallState == InstallationState.Applied);
                }

                return _launchCommand;
            }
        }

        public bool InstallEnabled => InstallCommand.CanExecute(this);

        public bool InstallVisible =>
            _mainModel.InstallState == InstallationState.Waiting
            || _mainModel.InstallState == InstallationState.Applying;

        public bool IsUpgrade
        {
            get { return _isUpgrade; }
            set { _isUpgrade = value; OnPropertyChanged(nameof(IsUpgrade)); }
        }

        public bool LaunchEnabled => LaunchCommand.CanExecute(this);

        public Control InstallView
        {
            get
            {
                switch (_mainModel.InstallState)
                {
                    case InstallationState.Applied:
                        return new InstallAppliedView();
                    case InstallationState.Applying:
                        return new ProgressView { DataContext = ProgressModel };
                    case InstallationState.Detecting:
                        break;
                    case InstallationState.Failed:
                        return new InstallFailedView { DataContext = InstallFailedModel };
                    case InstallationState.Initializing:
                        break;
                    case InstallationState.Planning:
                        break;
                    case InstallationState.Waiting:
                        return new InstallWaitingView { DataContext = InstallWaitingModel };
                }

                return null;
            }
        }

        public void Refresh()
        {
            InstallWaitingModel.Refresh();
        }

        private void LaunchApplication()
        {
            var target = _bootstrapper.Engine.StringVariables["LaunchTarget"];
            var formatted = _bootstrapper.Engine.FormatString(target);

            _bootstrapper.Engine.Log(LogLevel.Standard, $"Launching target: {formatted}");

            PicoBA.View.Close();

            Process.Start(formatted);
        }

        private void OnMainModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "DetectState"
                || e.PropertyName == "InstallState")
            {
                OnPropertyChanged(nameof(CancelEnabled));
                OnPropertyChanged(nameof(CloseEnabled));
                OnPropertyChanged(nameof(InstallEnabled));
                OnPropertyChanged(nameof(InstallView));
                OnPropertyChanged(nameof(InstallVisible));
                OnPropertyChanged(nameof(LaunchEnabled));
            }
        }

        private void OnResolveSource(object sender, ResolveSourceEventArgs e)
        {
            e.Result = InstallWaitingModel.ShouldDownloadDependencies
                ? Result.Download
                : Result.Ignore;
        }
    }
}
