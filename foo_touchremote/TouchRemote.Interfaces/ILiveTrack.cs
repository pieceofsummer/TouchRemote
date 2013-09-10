using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface ILiveTrack : ITrack
    {

        bool IsLiveStream { get; }

        string LiveArtistName { get; }

        string LiveAlbumName { get; }

        string LiveTitle { get; }

        string LiveGenreName { get; }

        string LiveComposerName { get; }

    }
}
