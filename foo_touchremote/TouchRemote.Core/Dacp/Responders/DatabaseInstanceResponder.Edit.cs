using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;
using System.Text.RegularExpressions;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http.Response;
using TouchRemote.Core.Filter;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder
    {

        private HttpResponse EditDatabaseResponse()
        {
            switch (Request.QueryString["action"])
            {
                case "add":
                    return AddPlaylist();

                case "remove":
                    return DeletePlaylist();

                default:
                    return null;
            }
        }

        private HttpResponse EditPlaylistResponse()
        {
            switch (Request.QueryString["action"])
            {
                case "add":
                    return AddTracks();

                case "remove":
                    return DeleteTracks();

                case "move":
                    return MoveTracks();

                default:
                    return null;
            }
        }

        private readonly Regex itemNameMask = new Regex("^'dmap\\.itemname:(.+)'$");
        private readonly Regex itemIdMask = new Regex("^'dmap\\.itemid:(\\d+)'$");

        private HttpResponse AddPlaylist()
        {
            var editParams = Request.QueryString["edit-params"];
            if (string.IsNullOrEmpty(editParams)) return null;

            var match = itemNameMask.Match(editParams);
            if (!match.Success) return null;

            var playlistName = match.Groups[1].Value.Replace("\\'", "'");

            using (Player.BeginWrite())
            {
                var pl = Player.CreatePlaylist(playlistName);
                if (pl == null) return null;

                return new DmapResponse(new
                {
                    medc = new
                    {
                        mstt = 200,
                        miid = pl.Id
                    }
                });
            }
        }

        private HttpResponse DeletePlaylist()
        {
            var editParams = Request.QueryString["edit-params"];
            if (string.IsNullOrEmpty(editParams)) return null;

            var match = itemIdMask.Match(editParams);
            if (!match.Success) return null;

            var playlistId = int.Parse(match.Groups[1].Value);

            using (Player.BeginWrite())
            {
                var playlist = Player.Playlists.Where(x => x.Id == playlistId).FirstOrDefault() as IEditablePlaylist;
                if (playlist != null)
                    playlist.Delete();
            }

            return new NoContentResponse();
        }



        private HttpResponse AddTracks()
        {
            // source is set by filter
            var editParams = Request.QueryString["edit-params"];
            if (!string.IsNullOrEmpty(editParams))
            {
                var filter = new FilterExpression<ITrack>(editParams);

                using (Player.BeginWrite())
                {
                    var playlist = Player.Playlists.Where(x => x.Id == id2.Value).FirstOrDefault() as IEditablePlaylist;
                    if (playlist != null)
                    {
                        using (Player.MediaLibrary.BeginRead())
                        {
                            var items = filter.Filter(Player.MediaLibrary.Tracks).Except(playlist.Tracks).ToArray();
                            if (items.Length > 0)
                                playlist.AddTrackRange(items);
                        }
                    }
                }
                
                return new NoContentResponse();
            }

            // source is set by playlist id
            var addSrc = Request.QueryString["edit-add-src"];
            int playlistId;
            if (!string.IsNullOrEmpty(addSrc) && int.TryParse(addSrc, out playlistId))
            {
                using (Player.BeginWrite())
                {
                    var source = Player.Playlists.Where(x => x.Id == playlistId).FirstOrDefault();
                    if (source != null)
                    {
                        var playlist = Player.Playlists.Where(x => x.Id == id2.Value).FirstOrDefault() as IEditablePlaylist;
                        if (playlist != null)
                        {
                            using (Player.MediaLibrary.BeginRead())
                            {
                                var items = source.Tracks.Except(playlist.Tracks).ToArray();
                                if (items.Length > 0)
                                    playlist.AddTrackRange(items);
                            }
                        }
                    }
                }

                return new NoContentResponse();
            }

            return null;
        }

        private readonly Regex containerItemIdMask = new Regex("^'dmap\\.containeritemid:(\\d+)'$");

        private HttpResponse DeleteTracks()
        {
            var editParams = Request.QueryString["edit-params"];
            if (string.IsNullOrEmpty(editParams)) return null;

            // HACK to use itemid instead of containeritemid
            var match = containerItemIdMask.Match(editParams);
            if (match.Success)
                editParams = string.Format("'dmap.itemid:{0}'", match.Groups[1].Value);

            var filter = new FilterExpression<ITrack>(editParams);

            using (Player.BeginWrite())
            {
                var playlist = Player.Playlists.Where(x => x.Id == id2.Value).FirstOrDefault() as IEditablePlaylist;
                if (playlist != null)
                {
                    using (Player.MediaLibrary.BeginRead())
                        playlist.RemoveTrackRange(filter.Filter(playlist.Tracks).ToArray());
                }
            }

            return new NoContentResponse();
        }

        private readonly Regex movePairMask = new Regex("^'edit-param\\.move-pair:(\\d+),(\\d+)'$");

        private HttpResponse MoveTracks()
        {
            var editParams = Request.QueryString["edit-params"];
            if (string.IsNullOrEmpty(editParams)) return null;

            var match = movePairMask.Match(editParams);
            if (!match.Success) return null;

            var idToMove = int.Parse(match.Groups[1].Value);
            var idMoveAfter = int.Parse(match.Groups[2].Value);

            using (Player.BeginWrite())
            {
                var playlist = Player.Playlists.Where(x => x.Id == id2.Value).FirstOrDefault() as IEditablePlaylist;
                if (playlist != null)
                {
                    var trackToMove = playlist.Tracks.IndexOfFirst(x => x.Id == idToMove);
                    if (trackToMove >= 0)
                    {
                        var moveAfter = (idMoveAfter == 0) ? -1 : playlist.Tracks.IndexOfFirst(x => x.Id == idMoveAfter);
                        if (moveAfter >= 0 || idMoveAfter == 0)
                        {
                            playlist.ReorderTracks(trackToMove, moveAfter);
                        }
                    }
                }
            }

            return new NoContentResponse();
        }

    }
}
