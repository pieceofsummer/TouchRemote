using System;
using System.Collections.Generic;
using System.Text;
using System.Collections.Specialized;
using System.Web;
using System.Net;

namespace TouchRemote.Core.Http
{
    public class HttpRequest
    {
        public IPEndPoint Client { get; private set; }

        public string Method { get; private set; }

        public string Path { get; private set; }

        public string Protocol { get; private set; }

        public string Version { get; private set; }

        public NameValueCollection Headers { get; private set; }

        public NameValueCollection QueryString { get; private set; }

        public byte[] PostData { get; private set; }

        private readonly HttpConnection m_connection;

        internal HttpRequest(IPEndPoint client, string method, string path, string protocol, string version, NameValueCollection headers, byte[] postData, HttpConnection connection)
        {
            m_connection = connection;

            Client = client;
            Method = method;

            var pathDelim = path.IndexOf('?');
            if (pathDelim == -1)
            {
                Path = HttpUtility.UrlDecode(path);
                QueryString = new NameValueCollection();
            }
            else
            {
                Path = HttpUtility.UrlDecode(path.Substring(0, pathDelim));
                QueryString = HttpUtility.ParseQueryString(path.Substring(pathDelim + 1));
            }

            Protocol = protocol;
            Version = version;
            Headers = headers;
            PostData = postData;
        }

        public bool IsClientConnected
        {
            get { return m_connection.IsClientConnected; }
        }

        internal HttpServer Server
        {
            get { return m_connection.Server; }
        }

        public void Dump()
        {
            Console.WriteLine("HTTP version: {0}", Version);
            Console.WriteLine("Path: {0}", Path);
            Console.WriteLine("Method: {0}", Method);

            Console.WriteLine("Parameters:");
            foreach (string hdr in QueryString)
                foreach (var item in QueryString.GetValues(hdr))
                    Console.WriteLine("  {0}: {1}", hdr, item);

            Console.WriteLine("Headers:");
            foreach (string hdr in Headers)
                foreach (var item in Headers.GetValues(hdr))
                    Console.WriteLine("  {0}: {1}", hdr, item);

            Console.WriteLine();
        }

    }
}
