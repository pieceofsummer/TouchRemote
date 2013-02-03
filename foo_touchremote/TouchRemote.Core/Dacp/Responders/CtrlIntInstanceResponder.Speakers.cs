using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class CtrlIntInstanceResponder
    {

        /// <summary>
        /// Answers the /ctrl-int/dbid/getspeakers request
        /// 
        /// Query string arguments are:
        /// session-id: id of the remote session;
        /// </summary>
        /// <returns>
        /// casp --+
        ///     mstt 4 200
        ///     mdcl --+
        ///         caia 1 1
        ///         cahp 1 0
        ///         cavd 1 1
        ///         minm 8 Computer
        ///         msma 8 0
        ///         cmvo 4 100
        ///     mdcl --+
        ///         ...
        /// </returns>
        private HttpResponse GetSpeakersResponse()
        {
            var speakers = new MultiValueTag();

            using (Player.BeginRead())
            {
                speakers.Add("mstt", 200);

                speakers.Add("mdcl", new
                {
                    caia = true,                    // is active?
                    cahp = false,                   // has protection? (lock icon)
                    cavd = true,                    // can play video? (screen or speaker icon)
                    minm = "Computer",              // name
                    msma = 0L,                      // id (will be passed to set-speakers)
                    cmvo = Player.CurrentVolume     // current volume
                });

                /*speakers.Add("mdcl", new
                {
                    caia = false,
                    cahp = false,
                    cavd = false,
                    minm = "Sennheiser HD600",
                    msma = 1L,
                    cmvo = 100
                });*/

            }

            return new DmapResponse(new
            {
                casp = speakers
            });
        }


    }
}
