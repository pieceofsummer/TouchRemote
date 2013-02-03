using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Misc;

namespace TouchRemote.Core.Filter
{
    public class SortExpression
    {
        private Func<IEnumerable<ITrack>, IEnumerable<ITrack>> expr;

        private IComparer<string> comparer = LatinFirstSortComparer.Instance;

        public SortExpression(string sort)
        {
            switch (sort)
	        {
                case "artist":
                case "artists":
                    expr = SortByArtist;
                    break;

                case "album":
                case "albums":
                    expr = SortByAlbum;
                    break;

                case "name":
                    expr = SortByName;
                    break;

		        default:
                    expr = SortByTitle;
                    break;
	        }
        }

        public IEnumerable<ITrack> Sort(IEnumerable<ITrack> source)
        {
            if (source == null || expr == null) return source;

            return expr(source);
        }

        private IEnumerable<ITrack> SortByArtist(IEnumerable<ITrack> source)
        {
            return source.OrderBy(x => x.AlbumArtistName, comparer).ThenBy(x => x.AlbumName).ThenBy(x => x.DiscNumber).ThenBy(x => x.TrackNumber).ThenBy(x => x.Title);
        }

        private IEnumerable<ITrack> SortByAlbum(IEnumerable<ITrack> source)
        {
            return source.OrderBy(x => x.AlbumName, comparer).ThenBy(x => x.DiscNumber).ThenBy(x => x.TrackNumber).ThenBy(x => x.Title);
        }

        private IEnumerable<ITrack> SortByName(IEnumerable<ITrack> source)
        {
            return source.OrderBy(x => x.Title, comparer);
        }

        private IEnumerable<ITrack> SortByTitle(IEnumerable<ITrack> source)
        {
            return source.OrderBy(x => x.DiscNumber).ThenBy(x => x.TrackNumber).ThenBy(x => x.Title).ThenBy(x => x.AlbumName).ThenBy(x => x.AlbumArtistName);
        }

    }

}
