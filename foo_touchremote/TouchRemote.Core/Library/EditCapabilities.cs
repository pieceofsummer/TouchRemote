using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Library
{
    [Flags]
    internal enum EditCapabilities : int
    {
        None = 0,
        AddTracks = 1,
        RemoveTracks = 2,
        ReorderTracks = 4,
        Refresh = 8,
        Smth1 = 16,
        Delete = 32,
        Smth2 = 64,
        Smth3 = 128
    }
}
