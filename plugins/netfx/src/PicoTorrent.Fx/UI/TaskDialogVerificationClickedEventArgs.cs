namespace PicoTorrent.UI
{
    public sealed class TaskDialogVerificationClickedEventArgs
    {
        public TaskDialogVerificationClickedEventArgs(bool isChecked)
        {
            IsChecked = isChecked;
        }

        public bool IsChecked { get; }
    }
}
