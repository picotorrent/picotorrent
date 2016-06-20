using System;

namespace PicoTorrent.UI
{
    public sealed class Window : IWindow
    {
        private readonly IntPtr _handle;

        public Window(IntPtr handle)
        {
            if (handle == null) throw new ArgumentNullException(nameof(handle));
            _handle = handle;
        }

        public IntPtr GetHandle()
        {
            return _handle;
        }
    }
}
