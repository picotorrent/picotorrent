using System;

namespace PicoTorrent
{
    [AttributeUsage(AttributeTargets.Assembly)]
    public sealed class PluginFactoryAttribute : Attribute
    {
        public PluginFactoryAttribute(Type factoryType)
        {
            if (factoryType == null) throw new ArgumentNullException(nameof(factoryType));
            Type = factoryType;
        }

        public Type Type { get; }
    }
}
