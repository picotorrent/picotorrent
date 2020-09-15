using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using PicoTorrentBootstrapper.Models;
using PicoTorrentBootstrapper.Views;
using System;
using System.ComponentModel;
using System.Windows.Controls;
using System.Windows.Input;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class InstallViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private readonly MainViewModel _mainModel;

        private ICommand _installCommand;


        public InstallViewModel(BootstrapperApplication bootstrapper, MainViewModel mainModel)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
            _mainModel = mainModel ?? throw new ArgumentNullException(nameof(mainModel));

            _bootstrapper.ApplyComplete += OnApplyComplete;
            _bootstrapper.DetectBegin += OnDetectBegin;
            _bootstrapper.DetectComplete += OnDetectComplete;
            _bootstrapper.DetectPackageComplete += OnDetectPackageComplete;
            _bootstrapper.ResolveSource += OnResolveSource;
            _bootstrapper.PlanComplete += OnPlanComplete;

            _mainModel.PropertyChanged += OnMainModelPropertyChanged;

            InstallApplyingModel = new InstallApplyingViewModel(bootstrapper);
            InstallWaitingModel = new InstallWaitingViewModel(bootstrapper);

        }

        public InstallApplyingViewModel InstallApplyingModel { get; private set; }
        public InstallWaitingViewModel InstallWaitingModel { get; private set; }

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

        public bool InstallEnabled
        {
            get { return _installCommand.CanExecute(this); }
        }

        public Control InstallView
        {
            get
            {
                switch (_mainModel.InstallState)
                {
                    case InstallationState.Applied:
                        return new InstallAppliedView();
                    case InstallationState.Applying:
                        return new InstallApplyingView { DataContext = InstallApplyingModel };
                    case InstallationState.Detecting:
                        break;
                    case InstallationState.Failed:
                        break;
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

        private void OnApplyComplete(object sender, ApplyCompleteEventArgs e)
        {
            _mainModel.Result = e.Status;

            // Set the state to applied or failed unless the state has already been set back to the preapply state
            // which means we need to show the UI as it was before the apply started.
            if (_mainModel.InstallState != _mainModel.PreApplyState)
            {
                _mainModel.InstallState = e.Status >= 0
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
            else if (e.Status >= 0 && _mainModel.PlannedAction == LaunchAction.UpdateReplace) // if we successfully applied an update close the window since the new Bundle should be running now.
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
            _mainModel.DetectState = e.Installed
                ? DetectionState.Present
                : DetectionState.Absent;
        }

        private void OnDetectComplete(object sender, DetectCompleteEventArgs e)
        {
            _mainModel.InstallState = InstallationState.Waiting;

            if (_bootstrapper.Command.Action == LaunchAction.Uninstall)
            {
                _mainModel.Plan(LaunchAction.Uninstall);
            }
            else if (e.Status >= 0)
            {
                if (_bootstrapper.Command.Display != Display.Full)
                {
                    _mainModel.Plan(_bootstrapper.Command.Action);
                }
            }
            else
            {
                _mainModel.InstallState = InstallationState.Failed;
            }

            // Force all commands to reevaluate CanExecute.
            // InvalidateRequerySuggested must be run on the UI thread.
            PicoBA.Dispatcher.Invoke(new Action(CommandManager.InvalidateRequerySuggested));
        }

        private void OnDetectPackageComplete(object sender, DetectPackageCompleteEventArgs e)
        {
            if (e.PackageId == "VC_redist.x64.exe"
                && e.State == PackageState.Present)
            {
            }
        }

        private void OnMainModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "DetectState" || e.PropertyName == "InstallState")
            {
                OnPropertyChanged("InstallEnabled");
                OnPropertyChanged("InstallView");
            }
        }

        private void OnPlanComplete(object sender, PlanCompleteEventArgs e)
        {
            if (e.Status >= 0)
            {
                _mainModel.PreApplyState = _mainModel.InstallState;
                _mainModel.InstallState = InstallationState.Applying;
                _bootstrapper.Engine.Apply(_mainModel.WindowHandle);
            }
            else
            {
                _mainModel.InstallState = InstallationState.Failed;
            }
        }

        private void OnResolveSource(object sender, ResolveSourceEventArgs e)
        {
            e.Result = Result.Download;
        }
    }
}
