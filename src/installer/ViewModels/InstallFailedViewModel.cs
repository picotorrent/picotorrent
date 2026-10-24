using WixToolset.BootstrapperApplicationApi;
using System;
using System.Diagnostics;
using System.Windows.Input;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class InstallFailedViewModel : PropertyNotifyBase
    {
        private readonly PicoBA _bootstrapper;
        private ICommand _openLogFilesCommand;

        public InstallFailedViewModel(PicoBA bootstrapper)
        {
            _bootstrapper = bootstrapper ?? throw new ArgumentNullException(nameof(bootstrapper));
        }

        public ICommand OpenLogFiles
        {
            get
            {
                if (_openLogFilesCommand == null)
                {
                    _openLogFilesCommand = new RelayCommand(
                        _ => ShowLogFiles());
                }

                return _openLogFilesCommand;
            }
        }

        private void ShowLogFiles()
        {
            var log = _bootstrapper.Engine.FormatString(_bootstrapper.Engine.GetVariableString("WixBundleLog"));

            var startInfo = new ProcessStartInfo
            {
                FileName = "explorer.exe",
                Arguments = $"/e, /select, \"{log}\""
            };

            Process.Start(startInfo);
        }
    }
}
