using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.IO;
using System.Collections.Specialized;
using System.Globalization;
using System.Text.RegularExpressions;
using System.Web;
using TouchRemote.Core.Http.Response;
using System.Diagnostics;
using System.Threading;
using System.Net;

namespace TouchRemote.Core.Http
{
    public class HttpConnection
    {
        private readonly HttpServer server;
        private readonly Socket client;

        internal HttpConnection(Socket client, HttpServer server)
        {
            this.server = server;
            this.client = client;
        }

        public bool IsClientConnected
        {
            get { return server.IsRunning && !server.IsStopping && client.Connected && client.Poll(0, SelectMode.SelectWrite); }
        }

        internal HttpServer Server { get { return server; } }

        public void Work()
        {
            var handler = server.RequestHandler;

            //using (var stream = new NetworkStream(client, false))
            //using (var reader = new StreamReader(stream))
            {
                var httpRequest = new HttpRequestHeader(this, client);

                while (IsClientConnected)
                {
                    try
                    {
                        var httpString = httpRequest.Read();

                        if (string.IsNullOrEmpty(httpString)) break;

                        NameValueCollection headers = null;
                        string method = null;
                        string path = null;
                        string protocol = null;
                        string version = null;
                        bool? hasPostData = null;

                        using (var reader = new StringReader(httpString))
                        {
                            var firstLine = reader.ReadLine();
                            if (string.IsNullOrEmpty(firstLine))
                            {
                                server.LogMessage("HTTP ERROR: Empty request");
                                SendInvalidRequest(null);
                                //continue;
                                break;
                            }

                            var m = Regex.Match(firstLine, @"^([a-zA-Z]+)\s+(\S+)\s+([A-Z]+)/(\d.\d)$");
                            if (!m.Success)
                            {
                                server.LogMessage("HTTP FIXME: {0}", firstLine);
                                SendInvalidRequest(null);
                                break;
                            }

                            method = m.Groups[1].Value.ToUpperInvariant();
                            path = m.Groups[2].Value;
                            protocol = m.Groups[3].Value;
                            version = m.Groups[4].Value;

                            hasPostData = server.HasPostData(method);
                            if (hasPostData == null)
                            {
                                server.LogMessage("HTTP ERROR: Unknown method {0}", method);
                                SendInvalidRequest("Unknown HTTP method", protocol, version);
                                break;
                            }
                            
                            if (!server.IsValidProtocol(protocol, version))
                            {
                                SendInvalidRequest("Unknown HTTP version", protocol, version);
                                break;
                            }

                            headers = new NameValueCollection(StringComparer.OrdinalIgnoreCase);
                            string header;
                            while (!string.IsNullOrEmpty(header = reader.ReadLine()))
                            {
                                var parts = header.Split(new[] { ':' }, 2);
                                if (parts.Length != 2) continue;

                                var headerName = parts[0].Trim();
                                var headerValue = parts[1].TrimStart();
                                if (string.IsNullOrEmpty(headerName) || string.IsNullOrEmpty(headerValue)) continue;

                                headers.Add(headerName, HttpUtility.UrlDecode(headerValue));
                            }
                        }

                        byte[] postData = null;

                        var contentLengthHdr = headers["Content-Length"];
                        int contentLength = 0;
                        if (!string.IsNullOrEmpty(contentLengthHdr) && int.TryParse(contentLengthHdr, NumberStyles.None, null, out contentLength) && contentLength > 0)
                        {
                            if (!hasPostData.Value)
                            {
                                server.LogMessage("{0} request cannot contain data", method);
                                SendInvalidRequest(method + " request cannot contain data", protocol, version);
                                break;
                            }

                            postData = new byte[contentLength];
                            //stream.Read(postData, 0, contentLength);
                            var offset = 0;
                            while (offset < contentLength)
                            {
                                var read = client.Receive(postData, offset, contentLength - offset, SocketFlags.None);
                                offset += read;
                            }
                        }

                        var request = new HttpRequest((IPEndPoint)client.RemoteEndPoint, method, path, protocol, version, headers, postData, this);
                        HttpResponse response;

                        try
                        {
                            response = handler(request);
                        }
                        catch (Exception ex)
                        {
                            server.LogError(ex);
                            response = new ServerErrorResponse(ex);
                        }

                        SendResponse(response);

                        if (response.Code >= 500) break;
                        
                        if (string.Equals(headers["Connection"], "close", StringComparison.OrdinalIgnoreCase)) break;

                        if (server.IsStopping) break;
                    }
                    catch (Exception ex)
                    {
                        server.LogMessage("HTTP ERROR: {0}", ex.ToString());
                        break;
                    }
                }

                //Server.Player.Logger.LogMessage("=== Connection {0} to be terminated ===", Thread.CurrentThread.ManagedThreadId);
                
                Close();
            }
        }

        public void Close()
        {
            client.Shutdown(SocketShutdown.Send);
            
            client.Blocking = false;

            var buff = new byte[1000];
            while (client.Available > 0) 
                client.Receive(buff);

            client.Close();
        }

        private void SendResponse(HttpResponse response)
        {
            response.Send(this);
        }

        private void SendResponse(HttpResponse response, string protocol, string version)
        {
            response.Send(this, protocol, version);
        }

        private void SendInvalidRequest(string message, string protocol, string version)
        {
            SendResponse(new InvalidRequestResponse(message), protocol, version);
        }

        private void SendInvalidRequest(string message)
        {
            SendResponse(new InvalidRequestResponse(message), "HTTP", "1.1");
        }

        internal void Send(byte[] data)
        {
            if (data == null || data.Length == 0) return;

            client.Send(data);
        }

        internal void Send(Encoding encoding, string data)
        {
            Send((encoding ?? Encoding.UTF8).GetBytes(data));
        }

        internal void SendFormat(Encoding encoding, string format, params object[] args)
        {
            Send(encoding, string.Format(format, args));
        }

    }
}
