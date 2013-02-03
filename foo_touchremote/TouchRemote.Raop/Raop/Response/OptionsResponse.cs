using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Raop.Response
{

    /// <summary>
    /// Processes OPTIONS requests
    /// 
    /// Request example:
    /// 
    /// OPTIONS * RTSP/1.0
    /// CSeq: 1
    /// User-Agent: iTunes/10.2.1 (Windows; Microsoft Windows Vista x64 x64 Ultimate Edition Service Pack 2 (Build 6002)) AppleWebKit/533.20.25
    /// Client-Instance: 42B1252D7B515746
    /// DACP-ID: 42B1252D7B515746
    /// Active-Remote: 2581871503
    /// Apple-Challenge: base64-encoded seed value
    /// 
    /// </summary>
    public class OptionsResponse : RaopResponse
    {

        public OptionsResponse(HttpRequest request) : base(request)
        {
            Headers["Public"] = string.Join(", ", RaopServer.Operations.Keys.ToArray());
        }

    }
}
