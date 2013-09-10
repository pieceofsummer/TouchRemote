using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;
using TouchRemote.Core.Http.Response;
using TouchRemote.Core.Filter;
using TouchRemote.Interfaces;
using TouchRemote.Core.Dacp.Queue;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class CtrlIntInstanceResponder
    {

        private HttpResponse QueueContentsResponse()
        {
            var queue = new QueueTracks();
            var track = Player.CurrentTrack;
            if (track != null)
            {
                var container = Player.ActivePlaylist ?? Player.MediaLibrary;

                var canp = new CanpData
                {
                    DatabaseId = id,
                    ContainerId = container.Id,
                    ContainerItemId = track.Id,
                    TrackId = track.Id
                };

                queue.Add(new QueueTrack()
                {
                    TrackSpecs = canp.Data,
                    AlbumId = (track.Album != null) ? track.Album.PersistentId : 0,
                    Album = track.AlbumName,
                    AlbumArtist = (track.AlbumArtist != null) ? track.AlbumArtist.Id : 0,
                    Artist = track.ArtistName,
                    Title = track.Title,
                    Genre = track.GenreName, 
                    MediaKind = 1, 
                    QueueIndex = 1,
                    Upcoming = false
                });
            }

            return new DmapResponse(new
            {
                ceQR = new
                {
                    mstt = 200,
                    muty = (byte)0,
                    ceQu = true, // update???
                    mlcl = queue,
                    apsm = false,
                    aprm = (byte)0
                }
            });
        }

        private HttpResponse QueueEditResponse()
        {
            var command = Request.QueryString["command"];
            int mode = int.Parse(Request.QueryString["mode"]);

            switch (command)
            {
                case "clear":
                    // todo: clear
                    return new NoContentResponse();

                case "add":
                    // todo: add
                    if (!string.IsNullOrEmpty(Request.QueryString["query"]))
                    {
                        var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
                        var queuefilter = Request.QueryString["queuefilter"];
                        var sort = new SortExpression(Request.QueryString["sort"]);

                        using (Player.MediaLibrary.BeginRead())
                        {
                            var items = new List<ITrack>();
                            var oldCount = 0;

                            if (mode == 3)
                            {
                                // next
                                var oldItems = Player.GetPlaybackSource();
                                if (oldItems != null)
                                    items.AddRange(oldItems);
                                oldCount = items.Count;
                            }

                            if (string.IsNullOrEmpty(queuefilter))
                            {
                                items.AddRange(sort.Sort(filter.Filter(Player.MediaLibrary.Tracks)));
                            }
                            else if (queuefilter.StartsWith("playlist:"))
                            {
                                var playlist = Player.Playlists.FirstOrDefault(x => x.Id == int.Parse(queuefilter.Substring(9)));
                                if (playlist != null)
                                    items.AddRange(sort.Sort(filter.Filter(playlist.Tracks)));
                                else
                                    throw new InvalidOperationException("Playlist with id " + queuefilter.Substring(9) + " not found");
                            }
                            else
                                throw new InvalidOperationException("Unknown queuefilter: " + queuefilter);

                            if (items.Count != oldCount)
                            {
                                Player.SetPlaybackSource(items.ToArray());
                            }
                        }
                    }

                    if (mode == 1)
                    {
                        int index;
                        if (string.IsNullOrEmpty(Request.QueryString["index"]) || !int.TryParse(Request.QueryString["index"], out index))
                            index = -1;

                        var track = Player.Play(index);
                    }

                    return new DmapResponse(new
                    {
                        cacr = new
                        {
                            mstt = 200,
                            miid = (Player.ActivePlaylist ?? Player.MediaLibrary).Id
                        }
                    });

                default:
                    return new NotFoundResponse();
            }
        }

    }
}
