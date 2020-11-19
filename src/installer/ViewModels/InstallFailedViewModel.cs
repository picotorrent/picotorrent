using Microsoft.Tools.WindowsInstallerXml.Bootstrapper;
using System;
using System.Diagnostics;
using System.Windows.Input;

namespace PicoTorrentBootstrapper.ViewModels
{
    public sealed class InstallFailedViewModel : PropertyNotifyBase
    {
        private readonly BootstrapperApplication _bootstrapper;
        private ICommand _openLogFilesCommand;

        public InstallFailedViewModel(BootstrapperApplication bootstrapper)
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
            var log = _bootstrapper.Engine.FormatString(_bootstrapper.Engine.StringVariables["WixBundleLog"]);

            var startInfo = new ProcessStartInfo
            {
                FileName = "explorer.exe",
                Arguments = $"/e, /select, \"{log}\""
            };

            Process.Start(startInfo);
        }
    }
}
