using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IPreferences
    {

        bool SortTracksAlphabetically { get; }

        bool CustomSortAlbums { get; }

        bool CustomSortArtists { get; }

        bool CompressNetworkTraffic { get; }

        bool AllowGuestLogins { get; }

    }
}
