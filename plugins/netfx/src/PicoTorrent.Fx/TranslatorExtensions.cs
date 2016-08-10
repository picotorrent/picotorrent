using System.Text.RegularExpressions;

namespace PicoTorrent
{
    public static class TranslatorExtensions
    {
        public static string Translate(this ITranslator translator, string key, params object[] parameters)
        {
            var translated = translator.Translate(key);
            var formatIndex = -1;

            var formattedString = Regex.Replace(translated, "%s", m =>
            {
                formatIndex += 1;
                return $"{{{formatIndex}}}";
            });

            return string.Format(formattedString, parameters);
        }
    }
}
