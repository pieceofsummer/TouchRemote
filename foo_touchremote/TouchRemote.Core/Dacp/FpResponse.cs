using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Net;
using System.IO;
using System.IO.Compression;

namespace TouchRemote.Core.Dacp
{
    public class FpResponse : HttpResponse
    {
        private readonly byte v1, v2, v3, v4;
        private readonly byte[] payload;

        public FpResponse(byte v1, byte v2, byte v3, byte v4, byte[] payload)
        {
            this.v1 = v1;
            this.v2 = v2;
            this.v3 = v3;
            this.v4 = v4;
            this.payload = payload;

            Code = 200;
            Reason = "OK";
            Headers["DAAP-Server"] = "TouchRemote v2";
            Headers["Content-Type"] = "application/octet-stream";
            Headers["Content-Encoding"] = "gzip";
            Headers["Connection"] = "keep-alive";
        }

        protected override byte[] GetData()
        {
            using (var ms = new MemoryStream(256))
            {
                using (var gzip = new GZipStream(ms, CompressionMode.Compress, true))
                using (var wr = new BinaryWriter(gzip))
                {
                    wr.Write(Encoding.ASCII.GetBytes("FPLY"));
                    wr.Write(v1);
                    wr.Write(v2);
                    wr.Write(v3);
                    wr.Write(v4);

                    if (payload != null && payload.Length > 0)
                    {
                        wr.Write(IPAddress.HostToNetworkOrder(payload.Length));
                        wr.Write(payload);
                    }
                    else
                    {
                        wr.Write(0);
                    }
                }

                return ms.ToArray();
            }
        }
    }
}
