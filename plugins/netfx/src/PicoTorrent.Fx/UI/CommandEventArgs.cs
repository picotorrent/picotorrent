namespace PicoTorrent.UI
{
    public sealed class CommandEventArgs
    {
        public CommandEventArgs(int id)
        {
            Id = id;
        }

        public int Id { get; }
    }
}
