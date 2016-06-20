using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace PicoTorrent
{
    public sealed class PluginAssemblyFinder
    {
        public class AttributeFinder : MarshalByRefObject
        {
            public bool HasAttribute(string assembly)
            {
                var asm = Assembly.LoadFrom(assembly);
                var attrs = asm.GetCustomAttributes(typeof(PluginFactoryAttribute));

                return attrs.Any();
            }
        }

        public IEnumerable<Assembly> Find(string searchPath)
        {
            var assemblies = Directory.GetFiles(searchPath, "*.dll", SearchOption.TopDirectoryOnly);

            if (assemblies == null || !assemblies.Any())
            {
                return Enumerable.Empty<Assembly>();
            }

            var domain = AppDomain.CreateDomain("PicoTorrent/PluginAssemblyFinder");
            var finder = (AttributeFinder)domain.CreateInstanceFromAndUnwrap(
                typeof(PluginAssemblyFinder).Assembly.Location,
                typeof(AttributeFinder).FullName);

            var result = new List<Assembly>();

            foreach (var assembly in assemblies)
            {
                if (!finder.HasAttribute(assembly))
                {
                    continue;
                }

                result.Add(Assembly.LoadFrom(assembly));
            }

            AppDomain.Unload(domain);

            return result;
        }
    }
}
