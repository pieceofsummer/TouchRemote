using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Http.Response;
using TouchRemote.Core.Filter;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class CtrlIntInstanceResponder
    {

        /// <summary>
        /// Answers the /ctrl-int/dbid/getproperty request
        /// 
        /// Query string arguments are:
        /// session-id: id of the remote session;
        /// properties: comma-separated list of properties to get. Remote seems using only 'volume'.
        /// </summary>
        /// <returns>
        /// cmgt --+
        ///     mstt 4 200 (status)
        ///     cmvo 4 100 (volume, 0-100)
        /// </returns>
        private HttpResponse GetPropertyResponse()
        {
            var allProps = Request.QueryString["properties"];

            var values = new Dictionary<string, object>();
            values["mstt"] = 200;

            if (!string.IsNullOrEmpty(allProps))
            {
                var props = allProps.Split(',');

                using (Player.BeginRead())
                {

                    var track = Player.CurrentTrack;

                    foreach (var prop in props)
                        switch (prop)
                        {
                            case "dmcp.volume":
                                values["cmvo"] = Player.CurrentVolume;
                                break;
                            case "dacp.playerstate":
                                values["caps"] = (byte)Player.CurrentState;
                                break;
                            case "dacp.nowplaying":
                                if (track != null)
                                {
                                    var container = Player.ActivePlaylist ?? Player.MediaLibrary;

                                    /*var containerItemId = container.Id;
                                    var trackList = container.Tracks as IList<ITrack>;
                                    if (trackList != null)
                                        containerItemId = container.Id + 1 + trackList.IndexOf(track);*/

                                    values["canp"] = new CanpData
                                    {
                                        DatabaseId = id,
                                        ContainerId = container.Id,
                                        ContainerItemId = track.Id,
                                        TrackId = track.Id
                                    }.Data;

                                    values["cann"] = track.Title;
                                    values["cana"] = track.ArtistName;
                                    values["canl"] = track.AlbumName;
                                    values["cang"] = track.GenreName;

                                    if (track.Album != null)
                                        values["asai"] = track.Album.PersistentId; 

                                    values["cmmk"] = 1;
                                }
                                break;
                            case "dacp.playingtime":
                                if (track != null)
                                {
                                    values["cast"] = (uint)Math.Round(track.Duration.TotalMilliseconds);
                                    values["cant"] = (uint)Math.Round((track.Duration - Player.CurrentPosition).TotalMilliseconds);
                                }
                                break;
                            case "dacp.volumecontrollable":
                                values["cavc"] = true;
                                break;
                            case "dacp.availableshufflestates":
                                values["caas"] = (int)Player.AvailableShuffleModes << 1;
                                break;
                            case "dacp.availablerepeatstates":
                                values["caar"] = (int)Player.AvailableRepeatModes << 1;
                                break;
                            case "dacp.shufflestate":
                                values["cash"] = (byte)Player.CurrentShuffleMode;
                                break;
                            case "dacp.repeatstate":
                                values["carp"] = (byte)Player.CurrentRepeatMode;
                                break;
                            case "dacp.fullscreenenabled":
                                values["cafe"] = false;
                                break;
                            case "dacp.fullscreen":
                                values["cafs"] = (byte)0;
                                break;
                            case "dacp.visualizerenabled":
                                values["cave"] = false;
                                break;
                            case "dacp.visualizer":
                                values["cavs"] = (byte)0;
                                break;
                        }
                }
            }

            return new DmapResponse(new { cmgt = values });
        }

        /// <summary>
        /// Answers the /ctrl-int/dbid/setproperty request
        /// 
        /// Query string arguments are:
        /// session-id: id of the remote session;
        /// 1 or more properties to set (property=value pairs):
        /// dacp.shufflestate - shuffle mode (0 = off, 1 = on)
        /// dacp.repeatstate - repeat mode (0 = off, 1 = track, 2 = all)
        /// dmcp.volume - volume (0-100)
        /// dacp.playingtime - playback time (ms)
        /// dacp.userrating - rating (0-100)
        /// </summary>
        /// <returns>
        /// returns no content (HTTP 204)
        /// </returns>
        private HttpResponse SetPropertyResponse()
        {
            foreach (string prop in Request.QueryString.Keys)
            {
                string value = Request.QueryString[prop];

                using (Player.BeginWrite())
                {
                    switch (prop)
                    {
                        case "dacp.shufflestate":
                            Player.CurrentShuffleMode = (ShuffleMode)byte.Parse(value);
                            break;
                        case "dacp.repeatstate":
                            Player.CurrentRepeatMode = (RepeatMode)byte.Parse(value);
                            break;
                        case "dmcp.volume":
                            Player.CurrentVolume = int.Parse(value);
                            break;
                        case "dacp.playingtime":
                            Player.CurrentPosition = TimeSpan.FromMilliseconds(uint.Parse(value));
                            break;
                        case "dacp.userrating":
                            var track = Player.CurrentTrack;
                            if (track != null)
                                track.Rating = (Rating)(int.Parse(value) / 20);
                            break;
                        case "com.apple.itunes.jukebox-vote":
                            VoteForTrack();
                            break;
                    }
                }
            }

            return new NoContentResponse();
        }

        private void VoteForTrack()
        {
            var itemSpec = Request.QueryString["item-spec"];
            if (string.IsNullOrEmpty(itemSpec)) return;

            using (Player.MediaLibrary.BeginRead())
            {
                var jb = Player.MediaLibrary.Jukebox as IJukeboxPlaylist;
                if (jb != null)
                {
                    var filter = new FilterExpression<ITrack>(itemSpec);

                    var tracks = filter.Filter(Player.MediaLibrary.Tracks);

                    foreach (var track in tracks)
                    {
                        jb.Vote(track);
                    }
                }
            }
        }

    }
}
