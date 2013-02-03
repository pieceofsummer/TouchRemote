using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Raop.Response
{

    /// <summary>
    /// Handles SETUP requests
    /// 
    /// Request sample:
    /// 
    /// SETUP rtsp://192.168.1.2/3385587764 RTSP/1.0
    /// CSeq: 2
    /// Transport: RTP/AVP/UDP;unicast;interleaved=0-1;mode=record;control_port=6001;timing_port=6002
    /// User-Agent: iTunes/10.2.1 (Windows; Microsoft Windows Vista x64 x64 Ultimate Edition Service Pack 2 (Build 6002)) AppleWebKit/533.20.25
    /// Client-Instance: 42B1252D7B515746
    /// DACP-ID: 42B1252D7B515746
    /// Active-Remote: 2491342592
    /// 
    /// </summary>
    public class SetupResponse : RaopResponse
    {

        public SetupResponse(HttpRequest request) : base(request)
        {

        }

    }
}
