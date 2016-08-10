using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using TinyIoC;

namespace PicoTorrent
{
    public sealed class PluginResolver
    {
        public IEnumerable<IPlugin> GetAll(TinyIoCContainer container, Assembly assembly)
        {
            var attr = assembly.GetCustomAttribute<PluginFactoryAttribute>();
            if (attr == null) { return Enumerable.Empty<IPlugin>(); }

            var registerMethod = attr.Type.GetMethod("Register", BindingFlags.Public | BindingFlags.Static);
            if (registerMethod == null) { return Enumerable.Empty<IPlugin>(); }

            var childContainer = container.GetChildContainer();
            var roc = new RegisterOnlyContainer(childContainer);

            registerMethod.Invoke(null, new object[] { roc });

            return new[] { childContainer.Resolve<IPlugin>() };
        }
    }
}
