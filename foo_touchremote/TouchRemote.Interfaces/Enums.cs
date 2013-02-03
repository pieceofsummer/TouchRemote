using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{

    public enum Rating
    {
        None,
        OneStar,
        TwoStars,
        ThreeStars,
        FourStars,
        FiveStars
    }

    public enum PlaybackState : byte
    {
        Stopped = 2,
        Paused = 3,
        Playing = 4,
    }

    [Flags]
    public enum RepeatMode
    {
        None,
        RepeatTrack,
        RepeatAll
    }

    [Flags]
    public enum ShuffleMode
    {
        None,
        Shuffle
    }

    public enum PlaylistType : byte
    {
        None = 0,
        Podcasts = 1,
        ITunesDJ = 2,
        Movies = 4,
        TVShows = 5,
        Music = 6,
        Books = 7,
        Purchased = 8,
        PurchasedOnDevice = 9,
        Rentals = 10,
        Genius = 12,
        ITunesU = 13,
    }

    [Flags]
    public enum MediaKind : byte
    {
        Track = 1,
        Music = 32
    }

}
