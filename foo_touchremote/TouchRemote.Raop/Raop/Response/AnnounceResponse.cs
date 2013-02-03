using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;
using System.IO;
using Org.BouncyCastle.Crypto.Encodings;
using TouchRemote.Core;
using Org.BouncyCastle.Crypto.Engines;
using System.Collections.Specialized;

namespace TouchRemote.Core.Raop.Response
{

    /// <summary>
    /// Processes ANNOUNCE requests
    /// 
    /// Request sample:
    /// 
    /// ANNOUNCE rtsp://192.168.1.2/3385586068 RTSP/1.0
    /// CSeq: 1
    /// Content-Type: application/sdp
    /// Content-Length: 568
    /// User-Agent: iTunes/10.2.1 (Windows; Microsoft Windows Vista x64 x64 Ultimate Edition Service Pack 2 (Build 6002)) AppleWebKit/533.20.25
    /// Client-Instance: 42B1252D7B515746
    /// DACP-ID: 42B1252D7B515746
    /// Active-Remote: 2300221844
    /// 
    /// v=0
    /// o=iTunes 3385586068 0 IN IP4 192.168.1.2
    /// s=iTunes
    /// c=IN IP4 192.168.1.2
    /// t=0 0
    /// m=audio 0 RTP/AVP 96
    /// a=rtpmap:96 AppleLossless
    /// a=fmtp:96 352 0 16 40 10 14 2 255 0 0 44100
    /// a=rsaaeskey:koNWenaXDH4y63oAGmZf8iVMXax2m6b0TT8frD2UG7jxcYQ1ryo8b0ttkJlRTzL34mvF3OFJhq7GpnesLkiJ2t8BbvuGC45sK8JOTXnlOY0YP4leQ8ne4lAw+agF3H0ytVytWJ+kS9zvvg04e6uIZaAaJg8wmEi8DYwhtc/FvkI9weAZbHshTHbFqT7c4NAYYyQ1S6BI/Qpn9Ks+58N09DQvs/ATyl26YaqEmM8RLDNoedNQqccyhqR+cPjn6EnX7rLu90HCi6cgM7qpDKjCYsGEMRG8c7cG6JH5hnz9L/wiZDndeb60x8w/BYemEyzBCh+2cCLDpow0bTiNBUbu4g
    /// a=aesiv:VxX+ZwI0OAGOmEsP+GItRw
    /// </summary>
    public class AnnounceResponse : RaopResponse
    {
        
        public AnnounceResponse(HttpRequest request) : base(request)
        {
            if (request.PostData == null)
                throw new InvalidOperationException("No POST data found");

            byte[] aesIV = null;
            byte[] aesKey = null;
            var values = new NameValueCollection();
            var separator = new char[] { ':' };

            using (var text = new StringReader(Encoding.ASCII.GetString(request.PostData)))
            {
                string line;
                while (null != (line = text.ReadLine()))
                {
                    if (!line.StartsWith("a=")) continue;

                    var parts = line.Substring(2).Split(separator, 2);
                    if (parts.Length != 2) continue;

                    values[parts[0]] = parts[1];
                }
            }

            if (!string.IsNullOrEmpty(values["rsaaeskey"]))
            {
                var rsa = new OaepEncoding(new RsaEngine());
                rsa.Init(false, rsaKey.Private);
                
                var data = values["rsaaeskey"].FromBase64();

                aesKey = rsa.ProcessBlock(data, 0, data.Length);
            }

            if (!string.IsNullOrEmpty(values["aesiv"]))
            {
                aesIV = values["aesiv"].FromBase64();
            }

        }

    }
}
