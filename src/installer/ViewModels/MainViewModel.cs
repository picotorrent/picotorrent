using Microsoft.Deployment.WindowsInstaller;
using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using PicoTorrentBootstrapper.Views;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class MainViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private readonly Window _mainWindow;

        private DetectionState _detectState;
        private InstallationState _installState;

        private bool _canceled;
        private bool _downgrade;
        private ICommand _cancelCommand;
        private ICommand _closeCommand;

        public MainViewModel(BootstrapperApplication ba, Window mainWindow)
        {
            _bootstrapper = ba ?? throw new ArgumentNullException(nameof(ba));
            _mainWindow = mainWindow ?? throw new ArgumentNullException(nameof(mainWindow));

            _bootstrapper.ApplyComplete += OnApplyComplete;
            _bootstrapper.DetectBegin += OnDetectBegin;
            _bootstrapper.DetectComplete += OnDetectComplete;
            _bootstrapper.DetectRelatedBundle += OnDetectRelatedBundle;
            _bootstrapper.DetectRelatedMsiPackage += OnDetectRelatedMsiPackage;
            _bootstrapper.PlanComplete += OnPlanComplete;

            WindowHandle = new WindowInteropHelper(_mainWindow).EnsureHandle();
            InstallModel = new InstallViewModel(ba, this);
            UninstallModel = new UninstallViewModel(ba, this);
        }

        public bool Canceled
        {
            get { return _canceled; }

            set
            {
                if (_canceled != value)
                {
                    _canceled = value;
                    OnPropertyChanged(nameof(Canceled));
                }
            }
        }

        public ICommand CancelCommand
        {
            get
            {
                if (_cancelCommand == null)
                {
                    _cancelCommand = new RelayCommand(_ =>
                        {
                            lock (this)
                            {
                                Canceled = MessageBox.Show(
                                    PicoBA.View,
                                    "Are you sure you want to cancel?",
                                    "PicoTorrent",
                                    MessageBoxButton.YesNo,
                                    MessageBoxImage.Error) == MessageBoxResult.Yes;
                            }
                        },
                        _ => InstallState == InstallationState.Applying);
                }

                return _cancelCommand;
            }
        }

        public ICommand CloseCommand
        {
            get
            {
                if (_closeCommand == null)
                {
                    _closeCommand = new RelayCommand(_ => PicoBA.View.Close());
                }

                return _closeCommand;
            }
        }

        public InstallViewModel InstallModel { get; private set; }

        public UninstallViewModel UninstallModel { get; private set; }

        public Control View
        {
            get
            {
                switch (DetectState)
                {
                    case DetectionState.Absent:
                        return new InstallView { DataContext = InstallModel };

                    case DetectionState.Newer:
                        return new NewerView { DataContext = new NewerViewModel(this) };

                    case DetectionState.Present:
                        return new UninstallView { DataContext = UninstallModel };
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
                    _bootstrapper.Engine.Log(LogLevel.Standard, $"Changing install state to {value} (prev {_installState})");
                    _installState = value;
                    OnPropertyChanged(nameof(InstallState));
                }
            }
        }

        public LaunchAction PlannedAction { get; set; }

        public InstallationState PreApplyState { get; set; }

        public IntPtr WindowHandle { get; set; }

        public int Result { get; internal set; }

        private void OnApplyComplete(object sender, ApplyCompleteEventArgs e)
        {
            Result = e.Status;

            // Set the state to applied or failed unless the state has already been set back to the preapply state
            // which means we need to show the UI as it was before the apply started.
            if (InstallState != PreApplyState)
            {
                InstallState = e.Status >= 0
                    ? InstallationState.Applied
                    : InstallationState.Failed;
            }

            // If we're not in Full UI mode, we need to alert the dispatcher to stop and close the window for passive.
            if (_bootstrapper.Command.Display != Display.Full)
            {
                // If its passive, send a message to the window to close.
                if (_bootstrapper.Command.Display == Display.Passive)
                {
                    _bootstrapper.Engine.Log(LogLevel.Verbose, "Automatically closing the window for non-interactive install");
                    PicoBA.Dispatcher.BeginInvoke(new Action(PicoBA.View.Close));
                }
                else
                {
                    PicoBA.Dispatcher.InvokeShutdown();
                }
                return;
            }
            else if (e.Status >= 0 && PlannedAction == LaunchAction.UpdateReplace) // if we successfully applied an update close the window since the new Bundle should be running now.
            {
                _bootstrapper.Engine.Log(LogLevel.Verbose, "Automatically closing the window since update successful.");
                PicoBA.Dispatcher.BeginInvoke(new Action(PicoBA.View.Close));
                return;
            }

            // Force all commands to reevaluate CanExecute.
            // InvalidateRequerySuggested must be run on the UI thread.
            PicoBA.Dispatcher.Invoke(new Action(CommandManager.InvalidateRequerySuggested));
        }

        private void OnDetectBegin(object sender, DetectBeginEventArgs e)
        {
            DetectState = e.Installed
                ? DetectionState.Present
                : DetectionState.Absent;
        }

        private void OnDetectComplete(object sender, DetectCompleteEventArgs e)
        {
            InstallState = InstallationState.Waiting;

            if (_bootstrapper.Command.Action == LaunchAction.Uninstall)
            {
                Plan(LaunchAction.Uninstall);
            }
            else if (e.Status >= 0)
            {
                if (_downgrade)
                {
                    DetectState = DetectionState.Newer;
                }

                if (_bootstrapper.Command.Display != Display.Full)
                {
                    Plan(_bootstrapper.Command.Action);
                }
            }
            else
            {
                InstallState = InstallationState.Failed;
            }

            // Force all commands to reevaluate CanExecute.
            // InvalidateRequerySuggested must be run on the UI thread.
            PicoBA.Dispatcher.Invoke(new Action(CommandManager.InvalidateRequerySuggested));

            InstallModel.Refresh();
        }

        private void OnDetectRelatedBundle(object sender, DetectRelatedBundleEventArgs e)
        {
            if (e.Operation == RelatedOperation.Downgrade)
            {
                _downgrade = true;
            }
        }

        private void OnDetectRelatedMsiPackage(object sender, DetectRelatedMsiPackageEventArgs e)
        {
            if (e.PackageId == "PicoTorrentPackage"
                && e.Operation == RelatedOperation.MajorUpgrade)
            {
                var existingProduct = new ProductInstallation(e.ProductCode);

                if (!existingProduct.IsInstalled)
                {
                    _bootstrapper.Engine.Log(LogLevel.Standard, $"Product not installed ({e.ProductCode})");
                    return;
                }

                try
                {
                    var firewallFeature = existingProduct.GetFeatureState("F_Firewall");
                    var associationFeature = existingProduct.GetFeatureState("F_Handlers");

                    InstallModel.InstallWaitingModel.AddWindowsFirewallException = firewallFeature == Microsoft.Deployment.WindowsInstaller.InstallState.Local;
                    InstallModel.InstallWaitingModel.RegisterFileProtocolHandlers = associationFeature == Microsoft.Deployment.WindowsInstaller.InstallState.Local;
                }
                catch (ArgumentException argException)
                {
                    _bootstrapper.Engine.Log(LogLevel.Error, $"Failed to get previous feature state: {argException}");
                }

                InstallModel.IsUpgrade = true;
            }
        }

        private void OnPlanComplete(object sender, PlanCompleteEventArgs e)
        {
            if (e.Status >= 0)
            {
                PreApplyState = InstallState;
                InstallState = InstallationState.Applying;
                _bootstrapper.Engine.Apply(WindowHandle);
            }
            else
            {
                InstallState = InstallationState.Failed;
            }
        }

        public void Plan(LaunchAction action)
        {
            PlannedAction = action;
            _bootstrapper.Engine.Plan(action);
        }
    }
}
