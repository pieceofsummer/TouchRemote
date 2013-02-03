using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Http.Response;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class CtrlIntInstanceResponder
    {

        /// <summary>
        /// Answers the /ctrl-int/dbid/nowplayingartwork request
        /// 
        /// Query string arguments are:
        /// session-id: id of the remote session;
        /// </summary>
        /// <returns>
        /// Image stream, or HTTP 204 if there's no artwork
        /// </returns>
        private HttpResponse NowPlayingArtworkResponse()
        {
            using (Player.BeginRead())
            {
                var track = Player.CurrentTrack as IArtworkSource;

                if (track == null) return new NoContentResponse();

                return Request.GetArtwork(new[] { track });
            }
        }

    }
}
