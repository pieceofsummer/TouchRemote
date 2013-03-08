using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using TouchRemote.Core.Http;
using System.Text.RegularExpressions;
using TouchRemote.Core.Http.Response;
using System.Collections.Specialized;
using System.Threading;
using System.IO;
using System.Collections;
using System.Net;
using TouchRemote.Interfaces;
using TouchRemote.Core.Filter;
using System.Globalization;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Answers the /ctrl-int/dbid/* requests
    /// </summary>
    internal partial class CtrlIntInstanceResponder : SessionBoundResponder
    {
        private static readonly Regex ctrlIntRegex = new Regex(@"^/ctrl-int/(\d+)/([a-z]+)$");

        private int id;
        private string query;

        public CtrlIntInstanceResponder(HttpRequest request) : base(request)
        {
            var match = ctrlIntRegex.Match(request.Path);
            if (match.Success)
            {
                id = int.Parse(match.Groups[1].Value);
                query = match.Groups[2].Value;
            }
            else
                throw new ArgumentException("Not a valid request for this responder", "request");
        }

        public override HttpResponse GetResponse()
        {
            switch (query)
            {
                case "getproperty":
                    return GetPropertyResponse();

                case "getspeakers":
                    return GetSpeakersResponse();
                //case "setspeakers":

                case "nowplayingartwork":
                    return NowPlayingArtworkResponse();

                case "playstatusupdate":
                    return PlayStatusUpdateResponse(); 

                case "playpause":
                case "pause":
                    Player.PlayPause();
                    return new NoContentResponse();
            }

            using (Session.LockState())
            {
                switch (query)
                {
                    case "setproperty":
                        return SetPropertyResponse();
                
                    case "playspec":
                        return PlaySpecResponse();

                    case "cue":
                        return CueResponse();

                    case "nextitem":
                        Player.PlayNext();
                        return new NoContentResponse();

                    case "previtem":
                        Player.PlayPrevious();
                        return new NoContentResponse();

                    //case "beginff":
                    //case "beginrew":
                }
            }

            return new NotFoundResponse();
        }

        /// <summary>
        /// Helper class to build Now Playing identifier (canp)
        /// </summary>
        private sealed class CanpData
        {
            private readonly byte[] m_data;

            public CanpData()
            {
                m_data = new byte[16];
            }

            public byte[] Data
            {
                get { return m_data; }
            }

            public int DatabaseId
            {
                get { return GetValue(0); }
                set { SetValue(0, value); }
            }

            public int ContainerId
            {
                get { return GetValue(4); }
                set { SetValue(4, value); }
            }

            public int ContainerItemId
            {
                get { return GetValue(8); }
                set { SetValue(8, value); }
            }

            public int TrackId
            {
                get { return GetValue(12); }
                set { SetValue(12, value); }
            }

            private int GetValue(int offset)
            {
                return (m_data[offset + 0] << 24) |
                       (m_data[offset + 1] << 16) |
                       (m_data[offset + 2] << 8) |
                       (m_data[offset + 3]);
            }

            private void SetValue(int offset, int value)
            {
                m_data[offset + 0] = (byte)((value >> 24) & 0xFF);
                m_data[offset + 1] = (byte)((value >> 16) & 0xFF);
                m_data[offset + 2] = (byte)((value >> 8) & 0xFF);
                m_data[offset + 3] = (byte)(value & 0xFF);
            }
        }

        private HttpResponse PlayStatusUpdateResponse()
        {
            var revisionStr = Request.QueryString["revision-number"];
            uint revisionNumber = 0;
            if (!string.IsNullOrEmpty(revisionStr) && uint.TryParse(revisionStr, out revisionNumber))
            {
                if (Session.CtrlIntRevision > 0)
                {
                    while (revisionNumber > Session.CtrlIntRevision)
                    {
                        Thread.Sleep(50);
                        if (!Request.IsClientConnected)
                        {
                            using (Player.BeginRead())
                                return new DmapResponse(new
                                {
                                    cmst = new
                                    {
                                        mstt = 200,
                                        cmsr = 0,
                                        caps = (byte)2,                                     // play status
                                        cash = (byte)Player.CurrentShuffleMode,             // shuffle status
                                        carp = (byte)Player.CurrentRepeatMode,              // repeat status
                                        cavc = true,                                        // 
                                        caas = (int)Player.AvailableShuffleModes << 1,
                                        caar = (int)Player.AvailableRepeatModes << 1,
                                        casu = false
                                    }
                                });
                        }
                    }
                }
                else
                    Session.CtrlIntRevision = 1;
            }

            using (Player.BeginRead())
            {
                var state = new Dictionary<string, object>()
                {
                    { "mstt", 200 },
                    { "cmsr", Session.CtrlIntRevision + 1 },
                    { "caps", (byte)Player.CurrentState },
                    { "cash", (byte)Player.CurrentShuffleMode },
                    { "carp", (byte)Player.CurrentRepeatMode },
                    { "cavc", true }, // volume controllable
                    { "caas", (int)Player.AvailableShuffleModes << 1 },
                    { "caar", (int)Player.AvailableRepeatModes << 1 },
                    { "casu", false },
                    /*
                    { "cavs", false },
                    { "cafs", false },
                    { "cafe", false },
                    { "cave", false }*/
                };

                var track = Player.CurrentTrack;

                if (track != null)
                {
                    var container = Player.ActivePlaylist ?? Player.MediaLibrary;

                    /*var containerItemId = container.Id;
                    var trackList = container.Tracks as IList<ITrack>;
                    if (trackList != null)
                        containerItemId = container.Id + 1 + trackList.IndexOf(track);*/

                    state["cmmk"] = 1;

                    state["canp"] = new CanpData
                    {
                        DatabaseId = id,
                        ContainerId = container.Id,
                        ContainerItemId = track.Id,
                        TrackId = track.Id
                    }.Data;

                    state["cann"] = track.Title;
                    state["cana"] = track.ArtistName;
                    state["canl"] = track.AlbumName;
                    state["cang"] = track.GenreName;

                    if (track.Album != null)
                        state["asai"] = track.Album.PersistentId;

                    state["cast"] = (uint)Math.Round(track.Duration.TotalMilliseconds);
                    state["cant"] = (uint)Math.Round((track.Duration - Player.CurrentPosition).TotalMilliseconds);
                    state["casu"] = true;
                }

                return new DmapResponse(new
                {
                    cmst = state
                });
            }
        }

        private HttpResponse CueResponse()
        {
            switch (Request.QueryString["command"])
            {
                case "clear":
                    {
                        Player.ClearPlaybackSource();

                        return new DmapResponse(new
                        {
                            cacr = new
                            {
                                mstt = 200,
                                miid = (Player.ActivePlaylist ?? Player.MediaLibrary).Id
                            }
                        });
                    }

                case "play":
                    {
                        if (!string.IsNullOrEmpty(Request.QueryString["query"]))
                        {
                            var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
                            var sort = new SortExpression(Request.QueryString["sort"]);

                            using (Player.MediaLibrary.BeginRead())
                            {
                                var items = sort.Sort(filter.Filter(Player.MediaLibrary.Tracks)).ToArray();
                                Player.SetPlaybackSource(items);
                            }
                        }

                        int index;
                        if (string.IsNullOrEmpty(Request.QueryString["index"]) || !int.TryParse(Request.QueryString["index"], out index))
                            index = -1;

                        var track = Player.Play(index);

                        return new DmapResponse(new
                        {
                            cacr = new
                            {
                                mstt = 200,
                                miid = (Player.ActivePlaylist ?? Player.MediaLibrary).Id
                            }
                        });
                    }

                case "add":
                    {
                        if (!string.IsNullOrEmpty(Request.QueryString["query"]))
                        {
                            var filter = new FilterExpression<ITrack>(Request.QueryString["query"]);
                            var sort = new SortExpression(Request.QueryString["sort"]);

                            using (Player.MediaLibrary.BeginRead())
                            {
                                var items = new List<ITrack>();
                                
                                var oldItems = Player.GetPlaybackSource();
                                if (oldItems != null)
                                    items.AddRange(oldItems);
                                var oldCount = items.Count;

                                items.AddRange(sort.Sort(filter.Filter(Player.MediaLibrary.Tracks)));
                                if (items.Count != oldCount)
                                {
                                    Player.SetPlaybackSource(items.ToArray());
                                }
                            }
                        }

                        return new DmapResponse(new
                        {
                            cacr = new
                            {
                                mstt = 200,
                                miid = (Player.ActivePlaylist ?? Player.MediaLibrary).Id
                            }
                        });
                    }

                default:
                    return new NotFoundResponse();
            }
        }

        private readonly Regex m_persistentIdMatch = new Regex("^'dmap\\.persistentid:0x([0-9a-fA-F]+)'$");
        private readonly Regex m_containerItemMatch = new Regex("^'dmap\\.containeritemid:0x([0-9a-fA-F]+)'$");

        private HttpResponse PlaySpecResponse()
        {
            /*var databaseSpec = Request.QueryString["database-spec"];

            if (string.IsNullOrEmpty(databaseSpec) || string.IsNullOrEmpty(containerSpec))
                return new InvalidRequestResponse("database-spec or container-spec is not set");

            var databaseMatch = m_persistentIdMatch.Match(databaseSpec);
            if (!databaseMatch.Success)
                return new InvalidRequestResponse("database-spec has invalid format");

            long dbId = (long)ulong.Parse(databaseMatch.Groups[1].Value, NumberStyles.HexNumber);*/

            var containerSpec = Request.QueryString["container-spec"];

            var containerMatch = m_persistentIdMatch.Match(containerSpec);
            if (!containerMatch.Success)
                return new InvalidRequestResponse("container-spec has invalid format");
            
            long ctId = (long)ulong.Parse(containerMatch.Groups[1].Value, NumberStyles.HexNumber);

            var shuffleState = Request.QueryString["dacp.shufflestate"];
            var itemSpec = Request.QueryString["container-item-spec"];

            var playlist = Player.GetContainers().FirstOrDefault(x => x.PersistentId == ctId);
            if (playlist == null)
                return new NotFoundResponse();

            bool isShuffle = "1".Equals(shuffleState);

            ITrack item = null;
            if (!isShuffle)
            {
                var itemMatch = m_containerItemMatch.Match(itemSpec);
                if (!itemMatch.Success)
                    return new InvalidRequestResponse("container-item-spec has invalid format");

                var itemId = (int)uint.Parse(itemMatch.Groups[1].Value, NumberStyles.HexNumber);

                item = playlist.Tracks.FirstOrDefault(x => x.Id == itemId);
                if (item == null)
                    return new NotFoundResponse();
            }

            if (playlist is IPlayableTrackCollection)
            {
                if (isShuffle)
                {
                    Player.CurrentShuffleMode = ShuffleMode.Shuffle;
                    ((IPlayableTrackCollection)playlist).PlayRandom();
                }
                else if (item != null)
                {
                    var trackIndex = ((IList<ITrack>)((IPlaylist)playlist).Tracks).IndexOf(item);
                    ((IPlayableTrackCollection)playlist).Play(trackIndex);
                }
            }
            else if (playlist is IJukeboxPlaylist)
            {
                if (item != null)
                {

                }
            }

            return PlayStatusUpdateResponse();
        }
    }
}
