using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Filter;
using TouchRemote.Core.Misc;
using TouchRemote.Core.Http.Response;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder
    {

        private HttpResponse GetGroupsResponse()
        {
            if (!id2.HasValue)
                return Groups();

            switch (query2)
            {
                case "extra_data/artwork":
                    return GroupArtwork();

                default:
                    return null;
            }
        }

        private HttpResponse Groups()
        {
            var groupType = Request.QueryString["group-type"];

            var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
            var includeSortHeaders = ("1".Equals(Request.QueryString["include-sort-headers"]));

            using (Player.MediaLibrary.BeginRead())
            {
                var rawItems = filter.Filter(Player.MediaLibrary.Tracks);

                switch (groupType)
                {
                    case "artist":
                    case "artists":
                        {
                            var items = rawItems.Where(x => x.AlbumArtist != null).GroupBy(x => x.AlbumArtist).OrderBy(x => x.Key.Name, LatinFirstSortComparer.Instance);

                            return new DmapResponse(new
                            {
                                agar = new
                                {
                                    mstt = 200,
                                    muty = (byte)0,
                                    mlcl = items.Select(x => new
                                    {
                                        miid = x.Key.Id,
                                        mper = x.Key.PersistentId,
                                        minm = x.Key.Name,
                                        agac = x.Where(t => t.Album != null).Select(t => t.Album).Distinct().Count(),
                                        mimc = x.Count()
                                    }).ToArray(),
                                    mshl = includeSortHeaders ? items.Select(x => x.Key.Name).GetShortcuts(x => x) : null
                                }
                            });
                        }

                    case "album":
                    case "albums":
                        {
                            var items = rawItems.Where(x => x.Album != null).GroupBy(x => x.Album).OrderBy(x => x.Key.Title, LatinFirstSortComparer.Instance).ThenBy(x => x.Key.Artist);

                            return new DmapResponse(new
                            {
                                agal = new
                                {
                                    mstt = 200,
                                    muty = (byte)0,
                                    mlcl = items.Select(x => new
                                    {
                                        miid = x.Key.Id,
                                        mper = x.Key.PersistentId,
                                        minm = x.Key.Title,
                                        asaa = x.Key.Artist.Name,
                                        astm = (uint)x.Sum(t => t.Duration.TotalMilliseconds),
                                        mimc = x.Count()
                                    }).ToArray(),
                                    mshl = includeSortHeaders ? items.Select(x => x.Key.Title).GetShortcuts(x => x) : null
                                }
                            });
                        }

                    default:
                        break;
                }
            }

            return new NotFoundResponse();
        }

        private HttpResponse GroupArtwork()
        {
            switch (Request.QueryString["group-type"])
            {
                case "artist":
                case "artists":
                    {
                        IArtworkSource[] items;

                        using (Player.MediaLibrary.BeginRead())
                        {
                            items = Player.MediaLibrary.Tracks.Where(x => x.AlbumArtist != null && x.AlbumArtist.Id == id2.Value).OfType<IArtworkSource>().ToArray();
                            return Request.GetArtwork(items);
                        }
                    }

                case "album":
                case "albums":
                    {
                        IArtworkSource[] items;

                        using (Player.MediaLibrary.BeginRead())
                        {
                            items = Player.MediaLibrary.Tracks.Where(x => x.Album != null && x.Album.Id == id2.Value).OfType<IArtworkSource>().ToArray();
                            return Request.GetArtwork(items);
                        }
                    }

                default:
                    return null;
            }
        }

    }
}
