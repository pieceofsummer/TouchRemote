using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IPlaylist : IReferenceItem, IEquatable<IPlaylist>
    {

        string Name { get; set; }

        PlaylistType Type { get; }

        int TrackCount { get; }

        IEnumerable<ITrack> Tracks { get; }

    }
}
