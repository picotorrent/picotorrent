namespace PicoTorrent
{
    public interface IConfiguration
    {
        T Get<T>(string key);
        void Set<T>(string key, T value);
    }
}
