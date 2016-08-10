namespace PicoTorrent
{
    public interface IContainer
    {
        void Register<TContract, TImplementation>()
            where TContract : class
            where TImplementation : class, TContract;

        void RegisterSingleton<TContract, TImplementation>()
            where TContract : class
            where TImplementation : class, TContract;
    }
}
