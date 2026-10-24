using WixToolset.BootstrapperApplicationApi;

namespace PicoTorrentBootstrapper
{
    internal static class Program
    {
        private static int Main()
        {
            var application = new PicoBA();

            ManagedBootstrapperApplication.Run(application);

            return 0;
        }
    }
}
