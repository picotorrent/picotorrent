using System;
using System.Windows.Forms;

namespace PicoTorrent.Plugins.AutoAdd.UI
{
    public static class ControlExtensions
    {
        public static void SafeInvoke(this Control control, Action action)
        {
            if (control == null)
            {
                throw new ArgumentNullException(nameof(control));
            }

            if (control.InvokeRequired)
            {
                control.Invoke((Action)delegate { SafeInvoke(control, action); });
            }
            else
            {
                if (control.IsDisposed)
                {
                    throw new ObjectDisposedException("Control is already disposed.");
                }

                action();
            }
        }

        public static T SafeInvoke<T>(this Control control, Func<T> func)
        {
            if (control == null)
            {
                throw new ArgumentNullException(nameof(control));
            }

            if (control.InvokeRequired)
            {
                return (T)control.Invoke((Action)delegate { SafeInvoke(control, func); });
            }
            else
            {
                if (control.IsDisposed)
                {
                    throw new ObjectDisposedException("Control is already disposed.");
                }

                return (T)func();
            }
        }
    }
}
