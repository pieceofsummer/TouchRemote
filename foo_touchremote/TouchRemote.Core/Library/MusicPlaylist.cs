using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Library
{
    public class MusicPlaylist : SpecialPlaylistBase, IPropertyExtender
    {
        public MusicPlaylist(IMediaLibrary library, string name) : base(library, PlaylistType.Music, name)
        {
        }

        public override IEnumerable<ITrack> Tracks
        {
            get { return m_library.Tracks; }
        }

        public override void Extend(IDictionary<string, object> data)
        {
            data["aeSP"] = true;
        }
    }
}
