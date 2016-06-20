using System;
using System.Diagnostics;
using PicoTorrent.Config;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.UpdateChecker.Dialogs
{
    public sealed class UpdateAvailableDialog : IUpdateAvailableDialog
    {
        private readonly IConfiguration _configuration;
        private readonly IMainWindow _mainWindow;
        private readonly ITranslator _translator;

        public UpdateAvailableDialog(IConfiguration configuration, IMainWindow mainWindow, ITranslator translator)
        {
            _configuration = configuration;
            _mainWindow = mainWindow;
            _translator = translator;
        }

        public void Show(Version version, string releaseUrl)
        {
            using (var dlg = _mainWindow.CreateTaskDialog())
            {
                const int ShowOnGitHub = 1337;

                dlg.ButtonClicked += (sender, args) =>
                {
                    if (args.ButtonId == ShowOnGitHub)
                    {
                        Process.Start(releaseUrl);
                    }
                };

                dlg.VerificationClicked += (sender, args) =>
                {
                    if (args.IsChecked)
                    {
                        _configuration.IgnoredUpdate = version.ToString();
                    }
                    else
                    {
                        _configuration.IgnoredUpdate = "";
                    }
                };

                dlg.Behavior = TaskDialogBehavior.PositionRelativeToWindow | TaskDialogBehavior.UseCommandLinks;
                dlg.CommonButtons = TaskDialogButton.Close;
                dlg.Content = _translator.Translate("new_version_available");
                dlg.MainIcon = TaskDialogIcon.Information;
                dlg.MainInstruction = _translator.Translate("picotorrent_v_available", version);
                dlg.Title = "PicoTorrent";
                dlg.VerificationText = _translator.Translate("ignore_update");

                dlg.AddButton(ShowOnGitHub, _translator.Translate("show_on_github"));
                dlg.Show();
            }
        }
    }
}
