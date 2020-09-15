using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using PicoTorrentBootstrapper.Views;
using System;
using System.Windows.Controls;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class MainViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _ba;

        private DetectionState _detectState;
        private InstallationState _installState;

        public MainViewModel(BootstrapperApplication ba, IntPtr hWndMain)
        {
            _ba = ba ?? throw new ArgumentNullException(nameof(ba));

            WindowHandle = hWndMain;
            InstallModel = new InstallViewModel(ba, this);
        }

        public InstallViewModel InstallModel { get; private set; }

        public Control View
        {
            get
            {
                switch (DetectState)
                {
                    case DetectionState.Absent:
                        return new InstallView { DataContext = InstallModel };
                    case DetectionState.Newer:
                        break;
                    case DetectionState.Present:
                        // uninstall
                        break;
                }

                return null;
            }
        }

        /// <summary>
        /// Gets and sets the detect state of the view's model.
        /// </summary>
        public DetectionState DetectState
        {
            get { return _detectState; }

            set
            {
                if (_detectState != value)
                {
                    _detectState = value;
                    OnPropertyChanged(nameof(DetectState));
                    OnPropertyChanged(nameof(View));
                }
            }
        }

        /// <summary>
        /// Gets and sets the installation state of the view's model.
        /// </summary>
        public InstallationState InstallState
        {
            get { return _installState; }

            set
            {
                if (_installState != value)
                {
                    _installState = value;
                    OnPropertyChanged(nameof(InstallState));
                }
            }
        }

        public LaunchAction PlannedAction { get; set; }

        public InstallationState PreApplyState { get; set; }

        public IntPtr WindowHandle { get; set; }

        public int Result { get; internal set; }

        public void Plan(LaunchAction action)
        {
            PlannedAction = action;
            _ba.Engine.Plan(action);
        }
    }
}
