using System;

namespace PicoTorrent.Plugins.Pushbullet.Net
{
    public sealed class PushbulletHttpException : Exception
    {
        public PushbulletHttpException(string message)
            : base(message)
        {

        }
    }
}
