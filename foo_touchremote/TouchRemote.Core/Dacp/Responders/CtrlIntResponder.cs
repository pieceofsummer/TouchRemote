using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Text.RegularExpressions;
using TouchRemote.Core.Http.Response;
using System.Collections.Specialized;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Answers the /ctrl-int requests
    /// </summary>
    internal class CtrlIntResponder : Responder
    {

        public CtrlIntResponder(HttpRequest request) : base(request) { }

        public override HttpResponse GetResponse()
        {
            return new DmapResponse(new
            {
                caci = new
                {
                    mstt = 200,
                    muty = (byte)0,
                    mlcl = new[]
                    {
                        new
                        {
                            miid = 1,
                            cmik = (byte)1,
                            cmpr = 0x00020001,
                            capr = 0x00020003,
                            cmsp = true,        // speakers selector (?)
                            aeFR = (byte)0x64,
                            cmsv = true,        // show volume control
                            cass = true,
                            caov = true,
                            casu = true,        // show rating stars
                            ceSG = false,       // show Genius button
                            cmrl = true
                        }
                    }
                }
            }); 
        }
    }
}
