using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IMediaLibrary : IPlaylist, IReadWriteObject
    {

        IEnumerable<IArtist> Artists { get; }

        IEnumerable<IAlbum> Albums { get; }

        IPlaylist Music { get; }

        IPlaylist Videos { get; }

        IPlaylist Podcasts { get; }

        IPlaylist AudioBooks { get; }

        IPlaylist Jukebox { get; }

    }
}
