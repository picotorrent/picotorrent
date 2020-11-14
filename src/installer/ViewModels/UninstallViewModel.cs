using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using PicoTorrentBootstrapper.Views;
using System;
using System.ComponentModel;
using System.Windows.Controls;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class UninstallViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private readonly MainViewModel _mainModel;

        public UninstallViewModel(BootstrapperApplication bootstrapper, MainViewModel mainModel)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
            _mainModel = mainModel ?? throw new ArgumentNullException(nameof(mainModel));
            _mainModel.PropertyChanged += OnMainModelPropertyChanged;

            ProgressModel = new ProgressViewModel(bootstrapper, mainModel);
        }

        public ProgressViewModel ProgressModel { get; private set; }

        public Control UninstallView
        {
            get
            {
                switch (_mainModel.InstallState)
                {
                    case InstallationState.Applied:
                        return new UninstallAppliedView();
                    case InstallationState.Applying:
                        return new ProgressView { DataContext = ProgressModel };
                    case InstallationState.Detecting:
                        break;
                    case InstallationState.Failed:
                        break;
                    case InstallationState.Initializing:
                        break;
                    case InstallationState.Planning:
                        break;
                    case InstallationState.Waiting:
                        return new UninstallWaitingView();
                }

                return null;
            }
        }

        private void OnMainModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "DetectState"
                || e.PropertyName == "InstallState")
            {
                OnPropertyChanged(nameof(UninstallView));
            }
        }
    }
}
