using PicoTorrent.UI;

namespace PicoTorrent.Plugins.UpdateChecker
{
    public sealed class NoUpdateAvailableDialog
    {
        private readonly IMainWindow _mainWindow;
        private readonly ITranslator _translator;

        public NoUpdateAvailableDialog(IMainWindow mainWindow, ITranslator translator)
        {
            _mainWindow = mainWindow;
            _translator = translator;
        }

        public void Show()
        {
            using (var dlg = _mainWindow.CreateTaskDialog())
            {
                dlg.Behavior = TaskDialogBehavior.PositionRelativeToWindow;
                dlg.CommonButtons = TaskDialogButton.Ok;
                dlg.MainIcon = TaskDialogIcon.Information;
                dlg.MainInstruction = _translator.Translate("no_update_available");
                dlg.Title = "PicoTorrent";

                dlg.Show();
            }
        }
    }
}
