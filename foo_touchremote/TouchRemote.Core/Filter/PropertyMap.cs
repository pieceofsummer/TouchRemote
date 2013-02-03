using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Filter
{
    internal sealed class TypeHintedReplacement
    {
        public TypeHintedReplacement(string name, bool needQuotes)
        {
            Name = name;
            NeedQuotes = needQuotes;
        }

        public string Name { get; set; }
        public bool NeedQuotes { get; set; }
    }

    internal static class PropertyMap
    {
        private static Dictionary<string, TypeHintedReplacement> m_replacements = new Dictionary<string, TypeHintedReplacement>(StringComparer.OrdinalIgnoreCase) {
            { "dmap.itemid", new TypeHintedReplacement("Id", false) },
            { "dmap.itemname", new TypeHintedReplacement("iif(Title != null, Title, \"\")", true) },
            { "dmap.containeritemid", new TypeHintedReplacement("Id", false) },
            { "daap.songalbum", new TypeHintedReplacement("iif(AlbumName != null, AlbumName, \"\")", true) },
            { "daap.songalbumid", new TypeHintedReplacement("iif(Album != null, Album.PersistentId, 0)", false) },
            { "daap.songartist", new TypeHintedReplacement("iif(ArtistName != null, ArtistName, \"\")", true) },
            { "daap.songalbumartist", new TypeHintedReplacement("iif(AlbumArtistName != null, AlbumArtistName, \"\")", true) },
            { "daap.songgenre", new TypeHintedReplacement("iif(GenreName != null, GenreName, \"\")", true) },
            { "daap.songcomposer", new TypeHintedReplacement("iif(ComposerName != null, ComposerName, \"\")", true) },
            { "com.apple.itunes.mediakind", new TypeHintedReplacement("Kind", false) },

            // ...
        };

        public static TypeHintedReplacement GetReplacement(string name)
        {
            TypeHintedReplacement r;
            if (m_replacements.TryGetValue(name, out r)) 
                return r;

            return null;
        }
    }
}
