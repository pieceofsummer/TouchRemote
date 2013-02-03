using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Globalization;

namespace TouchRemote.Core.Misc
{
    internal class PlaylistSortComparer : IComparer<string>
    {
        private static readonly PlaylistSortComparer instance = new PlaylistSortComparer();

        public static PlaylistSortComparer Instance
        {
            get { return instance; }
        }
        
        private readonly CompareInfo ci = CultureInfo.InvariantCulture.CompareInfo;

        public int Compare(string x, string y)
        {

            return ci.Compare(x, y, CompareOptions.IgnoreCase | CompareOptions.IgnoreSymbols);
        }

    }
}
