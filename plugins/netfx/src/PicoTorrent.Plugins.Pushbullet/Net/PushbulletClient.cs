using System;
using System.IO;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.Serialization.Json;
using System.Threading.Tasks;

using PicoTorrent.Plugins.Pushbullet.Net.Contracts;

namespace PicoTorrent.Plugins.Pushbullet.Net
{
    public sealed class PushbulletClient : IPushbulletClient
    {
        private static readonly string PushesUri = "https://api.pushbullet.com/v2/pushes";

        private readonly IPushbulletConfig _config;

        public PushbulletClient(IPushbulletConfig config)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));
            _config = config;
        }

        public async Task PushNoteAsync(string title, string body, string accessToken = "")
        {
            if (string.IsNullOrEmpty(title)) throw new ArgumentException("Title cannot be null or empty.", nameof(title));
            if (string.IsNullOrEmpty(body)) throw new ArgumentException("Body cannot be null or empty.", nameof(body));

            accessToken = string.IsNullOrEmpty(accessToken) ? _config.AccessToken : accessToken;

            using (var httpClient = new HttpClient())
            {
                httpClient.DefaultRequestHeaders.Add("Access-Token", accessToken);

                var note = new Note { Title = title, Body = body };
                var serializer = new DataContractJsonSerializer(typeof(Note));
                
                using (var ms = new MemoryStream())
                {
                    serializer.WriteObject(ms, note);

                    var content = new ByteArrayContent(ms.ToArray());
                    content.Headers.ContentType = new MediaTypeHeaderValue("application/json");

                    var responseMessage = await httpClient.PostAsync(PushesUri, content);
                    var b = await responseMessage.Content.ReadAsStringAsync();

                    if (!responseMessage.IsSuccessStatusCode)
                    {
                        throw new PushbulletHttpException(
                            $"Pushbullet returned HTTP status code {responseMessage.StatusCode}.");
                    }
                }
            }
        }
    }
}
