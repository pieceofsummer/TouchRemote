using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Filter;
using TouchRemote.Core.Misc;
using TouchRemote.Core.Library;
using System.Collections;
using System.Collections.Specialized;


namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder
    {

        #region Common methods

        private object GetPlaylistItem(IPlaylist p, int containerId)
        {    
            var bag = new Dictionary<string, object>()
            {
                { "miid", p.Id },
                { "mper", p.PersistentId },
                { "minm", p.Name },
                { "mpco", 0 },
                { "mimc", p.TrackCount },
                { "meds", (int)EditCapabilities.None }
            };

            if (p.Type != PlaylistType.None)
                bag["aePS"] = (byte)p.Type;
            else
                bag["meds"] = (int)(EditCapabilities.AddTracks | EditCapabilities.RemoveTracks | EditCapabilities.ReorderTracks | EditCapabilities.Delete);

            var ext = p as IPropertyExtender;
            if (ext != null)
                ext.Extend(bag);

            return bag;
        }

        private object GetTrackItem(ITrack t, int index)
        {
            var bag = new Dictionary<string, object>
            {
                { "mikd", (byte)2 },
                { "asar", t.ArtistName },
                { "asal", t.AlbumName },
                { "minm", t.Title },
                { "miid", t.Id },
                { "mper", t.PersistentId },
                { "asdb", false }, // song disabled
                { "mcti", t.Id }, //id2.Value + 1 + index,
                { "asai", t.Album != null ? t.Album.PersistentId : 0L },
                { "asri", t.AlbumArtist != null ? t.AlbumArtist.PersistentId : 0L },
                { "asaa", t.AlbumArtistName },
                { "astm", (uint)t.Duration.TotalMilliseconds },
                { "asur", (byte)((byte)t.Rating * 20) },
                { "aeHV", false },  // has video
                { "aeMk", t.Kind },
                { "mdst", true }
            };

            var ext = t as IPropertyExtender;
            if (ext != null)
                ext.Extend(bag);

            return bag;

            /*return new
            {
                mikd = (byte)2,
                asar = t.ArtistName,
                asal = t.AlbumName,
                miid = t.Id,
                minm = t.Title,
                mper = t.PersistentId,
                mcti = t.Id, //id2.Value + 1 + index,
                asai = (t.Album != null ? t.Album.PersistentId : 0L),
                astm = (uint)t.Duration.TotalMilliseconds,
                asur = (byte)((byte)t.Rating * 20),
                aeHV = false,
                aeMK = t.Kind
            };*/
        }

        #endregion

        private HttpResponse GetContainersResponse()
        {
            if (!id2.HasValue)
                return Containers();

            switch (query2)
            {
                case "items":
                    return ContainerItems();

                case "edit":
                    return EditPlaylistResponse();

                default:
                    return null;
            }
        }

        private HttpResponse Containers()
        {
            var items = new ArrayList(10);

            using (Player.MediaLibrary.BeginRead())
            {
                var mlib = Player.MediaLibrary;

                if (!Session.GuestMode)
                {
                    items.AddRange(Player.GetContainers().Select(x => GetPlaylistItem(x, mlib.Id)).ToArray());
                }
                else
                {
                    //items.Add(GetPlaylistItem(mlib, 0));
                    if (mlib.Music != null)
                        items.Add(GetPlaylistItem(mlib.Music, mlib.Id));
                    if (mlib.Jukebox != null)
                        items.Add(GetPlaylistItem(mlib.Jukebox, mlib.Id));
                }
            }

            return new DmapResponse(new
            {
                aply = new
                {
                    mstt = 200,
                    muty = (byte)0,
                    mlcl = items
                }
            });
        }

        private HttpResponse LibraryItems()
        {
            var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
            var includeSortHeaders = "1".Equals(Request.QueryString["include-sort-headers"]);
            var sort = new SortExpression(Request.QueryString["sort"]);

            using (Player.MediaLibrary.BeginRead())
            {
                var rawItems = sort.Sort(filter.Filter(Player.MediaLibrary.Tracks));

                Func<ITrack, string> selector;

                switch (Request.QueryString["sort"])
                {
                    case "album":
                    case "albums":
                        selector = x => x.AlbumArtistName;
                        break;

                    case "artist":
                    case "artists":
                        selector = x => x.AlbumName;
                        break;

                    default:
                        selector = x => x.Title;
                        break;
                }

                var items = rawItems.Select<ITrack, object>(GetTrackItem).ToArray();

                if (items.Length == 0 && !includeSortHeaders)
                {
                    // album not in library, and this is not search

                    using (Player.BeginRead())
                    {
                        if (filter.IsMatch(Player.CurrentTrack))
                            items = new[] { GetTrackItem(Player.CurrentTrack, 0) };
                    }
                }

                return new DmapResponse(new
                {
                    apso = new
                    {
                        mstt = 200,
                        muty = (byte)0,
                        mlcl = items,
                        //mshl = includeSortHeaders ? rawItems.GetShortcuts(selector) : null
                    }
                });
            }
        }

        private HttpResponse ContainerItems()
        {
            if (!id2.HasValue || id2.Value == 100)
                return LibraryItems();

            var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
            var includeSortHeaders = "1".Equals(Request.QueryString["include-sort-headers"]);
            var sort = new SortExpression(Request.QueryString["sort"]);

            using (Player.MediaLibrary.BeginRead())
            {
                var pl = Player.GetContainers().FirstOrDefault(x => x.Id == id2.Value);
                if (pl != null)
                {
                    lock (pl)
                    {
                        var rawItems = filter.Filter(pl.Tracks);

                        var items = rawItems.Select<ITrack, object>(GetTrackItem).ToArray();

                        return new DmapResponse(new
                        {
                            apso = new
                            {
                                mstt = 200,
                                muty = (byte)0,
                                mlcl = items
                            }
                        });
                    }
                }
            }

            return new DmapResponse(new
            {
                apso = new
                {
                    mstt = 200,
                    muty = (byte)0,
                    mlcl = new object[0]
                }
            });
        }

    }
}
