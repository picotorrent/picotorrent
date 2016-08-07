using System.Threading.Tasks;

namespace PicoTorrent.Plugins.Pushbullet.Net
{
    public interface IPushbulletClient
    {
        Task PushNoteAsync(string title, string body, string accessToken = "");
    }
}
