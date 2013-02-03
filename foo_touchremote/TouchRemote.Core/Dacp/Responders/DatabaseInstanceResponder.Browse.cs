using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Filter;
using TouchRemote.Core.Misc;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder
    {

        private HttpResponse GetBrowseResponse()
        {
            switch (query2)
            {
                case "genres":
                    return BrowseGenres();

                case "artists":
                    return BrowseArtists();

                case "composers":
                    return BrowseComposers();

                default:
                    return null;
            }
        }

        private HttpResponse BrowseGenres()
        {
            var filter = new FilterExpression<ITrack>(Request.QueryString["filter"]);
            var includeSortHeaders = "1".Equals(Request.QueryString["include-sort-headers"]);

            using (Player.MediaLibrary.BeginRead())
            {
                var rawItems = filter.Filter(Player.MediaLibrary.Tracks);

                var items = rawItems.Where(x => !string.IsNullOrEmpty(x.GenreName))
                                    .GroupBy(x => x.GenreName, StringComparer.InvariantCultureIgnoreCase)
                                    .Select(x => x.Key)
                                    .OrderBy(x => x, LatinFirstSortComparer.Instance)
                                    .ToArray();

                return new DmapResponse(new
                {
                    abro = new
                    {
                        mstt = 200,
                        muty = (byte)0,
                        abgn = items,
                        mshl = includeSortHeaders ? items.GetShortcuts(x => x) : null
                    }
                });
            }
        }

        private HttpResponse BrowseArtists()
        {
            var filter = new FilterExpression<ITrack>(Request.QueryString["filter"]);
            var includeSortHeaders = "1".Equals(Request.QueryString["include-sort-headers"]);

            using (Player.MediaLibrary.BeginRead())
            {
                var rawItems = filter.Filter(Player.MediaLibrary.Tracks);

                var items = rawItems.Where(x => !string.IsNullOrEmpty(x.ArtistName))
                                    .GroupBy(x => x.ArtistName, StringComparer.InvariantCultureIgnoreCase)
                                    .Select(x => x.Key)
                                    .OrderBy(x => x, LatinFirstSortComparer.Instance)
                                    .ToArray();

                return new DmapResponse(new
                {
                    abro = new
                    {
                        mstt = 200,
                        muty = (byte)0,
                        abar = items,
                        mshl = includeSortHeaders ? items.GetShortcuts(x => x) : null
                    }
                });
            }
        }

        private HttpResponse BrowseComposers()
        {
            var filter = new FilterExpression<ITrack>(Request.QueryString["filter"]);
            var includeSortHeaders = "1".Equals(Request.QueryString["include-sort-headers"]);

            using (Player.MediaLibrary.BeginRead())
            {
                var rawItems = filter.Filter(Player.MediaLibrary.Tracks);

                var items = rawItems.Where(x => !string.IsNullOrEmpty(x.ComposerName))
                                    .GroupBy(x => x.ComposerName, StringComparer.InvariantCultureIgnoreCase)
                                    .Select(x => x.Key)
                                    .OrderBy(x => x, LatinFirstSortComparer.Instance)
                                    .ToArray();

                return new DmapResponse(new
                {
                    abro = new
                    {
                        mstt = 200,
                        muty = (byte)0,
                        abcp = items,
                        mshl = includeSortHeaders ? items.GetShortcuts(x => x) : null
                    }
                });
            }
        }

    }
}
