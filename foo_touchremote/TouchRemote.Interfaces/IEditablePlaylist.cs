using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IEditablePlaylist : IPlaylist
    {
        void Delete();

        void Clear();

        void AddTrack(ITrack track);

        void AddTrackRange(IEnumerable<ITrack> tracks);

        void RemoveTrack(ITrack track);

        void RemoveTrackRange(IEnumerable<ITrack> tracks);

        void ReorderTracks(int trackToMove, int moveAfter);
    }

}
