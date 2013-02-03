using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.Specialized;
using System.Net.Sockets;
using System.Web;

namespace TouchRemote.Core.Http
{
    public abstract class HttpResponse
    {

        protected virtual string Protocol { get { return "HTTP"; } }

        protected virtual string Version { get { return "1.1"; } }

        public ushort Code { get; protected set; }

        public string Reason { get; protected set; }

        public NameValueCollection Headers { get; private set; }

        public HttpServer ServerContext { get; private set; }

        protected HttpResponse()
        {
            Headers = new NameValueCollection(StringComparer.OrdinalIgnoreCase);

            Headers["Content-Type"] = "text/plain; charset=utf-8";
        }

        internal void Send(HttpConnection connection)
        {
            Send(connection, Protocol, Version);
        }

        internal void Send(HttpConnection connection, string protocol, string version)
        {
            ServerContext = connection.Server;

            byte[] data = null;
            if (Code < 200 || Code == 204 || Code == 304)
            {
                Headers.Remove("Content-Type");
                Headers["Content-Length"] = "0";
            }
            else
            {
                data = GetData();
                Headers["Content-Length"] = (data != null) ? data.Length.ToString() : "0";
            }

            if (Code >= 500)
                Headers["Connection"] = "close";

            var hdrBuffer = new StringBuilder();
            hdrBuffer.AppendFormat("{0}/{1} {2} {3}\r\n", protocol, version, Code, Reason);
            foreach (string hdr in Headers)
		        foreach (var value in Headers.GetValues(hdr))
		            hdrBuffer.AppendFormat("{0}: {1}\r\n", hdr, value);
            
            hdrBuffer.Append("\r\n");

            connection.Send(Encoding.ASCII, hdrBuffer.ToString());
            connection.Send(data);
        }

        protected abstract byte[] GetData();

    }

    public abstract class TextHttpResponse : HttpResponse
    {

        protected abstract string GetText();

        protected sealed override byte[] GetData()
        {
            string data = GetText();
            if (string.IsNullOrEmpty(data))
                return null;

            return Encoding.UTF8.GetBytes(data);
        }

    }
}
