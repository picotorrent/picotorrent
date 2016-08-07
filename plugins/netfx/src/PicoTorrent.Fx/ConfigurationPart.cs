using System;
using System.Collections.Generic;
using System.Web.Script.Serialization;

namespace PicoTorrent
{
    public abstract class ConfigurationPart
    {
        private readonly IConfiguration _config;
        private readonly string _partName;
        private readonly IDictionary<string, object> _part;

        protected ConfigurationPart(IConfiguration configuration, string partName)
        {
            if (configuration == null) throw new ArgumentNullException(nameof(configuration));
            if (partName == null) throw new ArgumentNullException(nameof(partName));

            _config = configuration;
            _partName = partName;
            _part = _config.Get<IDictionary<string, object>>(partName) ?? new Dictionary<string, object>();
        }

        protected T Get<T>(string key, T defaultValue)
        {
            if (!_part.ContainsKey(key)) return defaultValue;

            var serializer = new JavaScriptSerializer();
            var json = serializer.Serialize(_part[key]);

            return serializer.Deserialize<T>(json);
        }

        protected void Set<T>(string key, T value)
        {
            if (!_part.ContainsKey(key)) _part.Add(key, value);
            _part[key] = value;
            _config.Set(_partName, _part);
        }
    }
}
