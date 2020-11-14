using System.Diagnostics;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Navigation;

namespace PicoTorrentBootstrapper.Views
{
    /// <summary>
    /// Interaction logic for InstallAppliedView.xaml
    /// </summary>
    public partial class InstallAppliedView : UserControl
    {
        public InstallAppliedView()
        {
            InitializeComponent();
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            var link = sender as Hyperlink;
            Process.Start(link.NavigateUri.ToString());
        }
    }
}
