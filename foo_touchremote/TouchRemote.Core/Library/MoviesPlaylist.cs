using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Library
{
    public class MoviesPlaylist : SpecialPlaylistBase
    {
        private readonly int m_id;
        private readonly string m_name;
        private readonly IMediaLibrary m_library;

        public MoviesPlaylist(IMediaLibrary library, string name) : base(library, PlaylistType.Movies, name)
        {
        }
        
        public override IEnumerable<ITrack> Tracks
        {
            get { return null; }
        }

    }
}
