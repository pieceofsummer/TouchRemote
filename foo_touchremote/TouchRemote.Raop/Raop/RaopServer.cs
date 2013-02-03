using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Raop
{
    public class RaopServer : HttpServer
    {

        internal static readonly IDictionary<string, bool> Operations = new Dictionary<string, bool>(StringComparer.OrdinalIgnoreCase)
        {
            { "OPTIONS", false }, 
            { "ANNOUNCE", true }, 
            { "SETUP", false }, 
            { "RECORD", false }, 
            { "PAUSE", false }, 
            { "FLUSH", false }, 
            { "TEARDOWN", false }, 
            { "GET_PARAMETER", false }, 
            { "SET_PARAMETER", true }
        };
        
        public RaopServer(ushort port) : base(port)
        {

        }

        protected internal override bool IsValidProtocol(string protocol, string version)
        {
            if (!string.Equals(protocol, "RTSP", StringComparison.OrdinalIgnoreCase))
                return false;

            return version == "1.0";
        }

        protected internal override bool? HasPostData(string method)
        {
            bool value;
            if (Operations.TryGetValue(method, out value))
                return value;

            return null;
        }

    }
}
