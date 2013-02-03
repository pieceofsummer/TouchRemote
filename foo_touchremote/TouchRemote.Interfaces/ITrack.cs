using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface ITrack : IReferenceItem, IComparable<ITrack>, IEquatable<ITrack>
    {

        IPlaybackSource Source { get; }
        
        IArtist AlbumArtist { get; }

        string ArtistName { get; }

        string AlbumArtistName { get; }

        IAlbum Album { get; }

        string AlbumName { get; }

        string Title { get; }

        string GenreName { get; }

        string ComposerName { get; }

        TimeSpan Duration { get; }

        Rating Rating { get; set; }

        byte Kind { get; }

        int DiscNumber { get; }

        int TrackNumber { get; }

    }
}
