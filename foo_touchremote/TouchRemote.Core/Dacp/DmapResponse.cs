using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Reflection;
using System.Collections;
using System.IO;
using System.Net;
using System.Diagnostics;

namespace TouchRemote.Core.Dacp
{
    public class DmapResponse : HttpResponse
    {
        private readonly object m_value;

        public DmapResponse(object value) 
        {
            if (value == null)
                throw new ArgumentNullException("value");

            m_value = value;
            
            Code = 200;
            Reason = "OK";
            Headers["DAAP-Server"] = "TouchRemote v2";
            Headers["Content-Type"] = "application/x-dmap-tagged";
            Headers["Connection"] = "keep-alive";
            //Headers["Content-Encoding"] = (withCompression) ? "gzip" : "binary/octet-stream";
        }

        protected override byte[] GetData()
        {
            var dacpServer = ServerContext as Dacp.DacpServer;

            var withCompression = (dacpServer != null) ? dacpServer.Player.Preferences.CompressNetworkTraffic : false;
            Headers["Content-Encoding"] = (withCompression) ? "gzip" : "binary/octet-stream";
            return DataSerializer.Serialize(m_value, withCompression);
        }

    }
}
