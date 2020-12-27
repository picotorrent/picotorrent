using System;
using System.ComponentModel;
using System.Diagnostics;

namespace PicoTorrentBootstrapper
{
    public abstract class PropertyNotifyBase : INotifyPropertyChanged
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PropertyNotifyBase"/> class.
        /// </summary>
        protected PropertyNotifyBase()
        {
        }

        /// <summary>
        /// Raised when a property on this object has a new value.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Warns the developer if this object does not have a public property with the
        /// specified name. This method does not exist in a Release build.
        /// </summary>
        /// <param name="propertyName">Property name to verify.</param>
        [Conditional("DEBUG")]
        [DebuggerStepThrough]
        public void VerifyPropertyName(string propertyName)
        {
            // Verify that the property name matches a real, public, instance property
            // on this object.
            if (null == TypeDescriptor.GetProperties(this)[propertyName])
            {
                Debug.Fail(String.Concat("Invalid property name: ", propertyName));
            }
        }

        /// <summary>
        /// Raises this object's PropertyChanged event.
        /// </summary>
        /// <param name="propertyName">The property that has a new value.</param>
        protected virtual void OnPropertyChanged(string propertyName)
        {
            this.VerifyPropertyName(propertyName);

            PropertyChangedEventHandler handler = this.PropertyChanged;
            if (null != handler)
            {
                PropertyChangedEventArgs e = new PropertyChangedEventArgs(propertyName);
                handler(this, e);
            }
        }
    }
}
