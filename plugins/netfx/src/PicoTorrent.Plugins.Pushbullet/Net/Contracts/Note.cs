using System.Runtime.Serialization;

namespace PicoTorrent.Plugins.Pushbullet.Net.Contracts
{
    [DataContract]
    public class Note
    {
        [DataMember(Name = "body")]
        public string Body { get; set; }

        [DataMember(Name = "title")]
        public string Title { get; set; }

        [DataMember(Name = "type")]
        public string Type { get; set; } = "note";
    }
}