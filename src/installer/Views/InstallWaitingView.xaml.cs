using System.Windows.Controls;
using System.Windows.Documents;

namespace PicoTorrentBootstrapper.Views
{
    /// <summary>
    /// Interaction logic for InstallConfigView.xaml
    /// </summary>
    public partial class InstallWaitingView : UserControl
    {
        public InstallWaitingView()
        {
            InitializeComponent();
        }

        private void Hyperlink_RequestNavigate(object sender, System.Windows.Navigation.RequestNavigateEventArgs e)
        {
            var link = sender as Hyperlink;
            PicoBA.LaunchProcess(link.NavigateUri.ToString());
        }
    }
}
