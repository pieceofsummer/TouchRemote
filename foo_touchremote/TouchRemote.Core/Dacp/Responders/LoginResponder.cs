using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Net;
using System.Globalization;
using TouchRemote.Core.Http.Response;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Answers the /login requests
    /// </summary>
    internal class LoginResponder : Responder
    {
        bool guestMode;

        public LoginResponder(HttpRequest request) : base(request)
        {
            guestMode = false;

            var guidStr = request.QueryString["pairing-guid"];
            if (!string.IsNullOrEmpty(guidStr))
            {
                if (!guidStr.StartsWith("0x"))
                    throw new ArgumentException("Invalid pairing guid requested", "request");

                var guid = (ulong)IPAddress.NetworkToHostOrder(long.Parse(guidStr.Substring(2), NumberStyles.AllowHexSpecifier));

                var pairedDevice = Pairing.PairedDevices.GetDevice(guid);
                if (pairedDevice == null)
                    throw new ArgumentException("Device is not paired with", "request");
            }
            else
            {
                var lStr = request.QueryString["l"];
                var kStr = request.QueryString["k"];
                if (string.IsNullOrEmpty(lStr) || string.IsNullOrEmpty(kStr))
                    throw new ArgumentException("Invalid login request", "request");

                guestMode = true;
            }
        }

        public override HttpResponse GetResponse()
        {
            if (guestMode && !Player.Preferences.AllowGuestLogins)
            {
                return new NotFoundResponse();
            }

            int sessionId = Math.Abs(unchecked((int)~DateTime.UtcNow.ToUnixTime()));

            SessionManager.StartSession(sessionId, guestMode);

            return new DmapResponse(new
            {
                mlog = new
                {
                    mstt = 200,
                    mlid = sessionId
                }
            });
        }

    }
}
