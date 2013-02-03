using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IPlayer : IReadWriteObject
    {
        
        string Name { get; }

        Version Version { get; }

        ILogger Logger { get; }

        ulong DatabaseId { get; }

        IPreferences Preferences { get; }

        IMediaLibrary MediaLibrary { get; }

        IEnumerable<IPlaylist> Playlists { get; }

        // -----------------------

        RepeatMode AvailableRepeatModes { get; }

        RepeatMode CurrentRepeatMode { get; set; }

        ShuffleMode AvailableShuffleModes { get; }

        ShuffleMode CurrentShuffleMode { get; set; }

        int CurrentVolume { get; set; }

        PlaybackState CurrentState { get; }

        TimeSpan CurrentPosition { get; set; }

        ITrack CurrentTrack { get; }

        // -----------------------

        IPlaylist ActivePlaylist { get; }

        void ClearPlaybackSource();

        void SetPlaybackSource(ITrack[] tracks);

        ITrack Play(int index);

        void PlayPause();

        void PlayNext();

        void PlayPrevious();

        IPlaylist CreatePlaylist(string name);
    }
}
