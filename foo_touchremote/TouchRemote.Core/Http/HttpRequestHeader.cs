using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Diagnostics;
using System.IO;

namespace TouchRemote.Core.Http
{
    internal class HttpRequestHeader
    {
        private readonly HttpConnection connection;
        private readonly Socket socket;
        private readonly StringBuilder buffer;
        private readonly byte[] portion;

        public HttpRequestHeader(HttpConnection connection, Socket socket)
        {
            if (connection == null)
                throw new ArgumentNullException("connection");
            if (socket == null)
                throw new ArgumentNullException("socket");

            this.connection = connection;
            this.socket = socket;
            this.buffer = new StringBuilder(1024);
            this.portion = new byte[1024];
        }

        private static int FindSubstringEnd(StringBuilder buffer, string pattern, int startIndex)
        {
            if (buffer == null)
                throw new ArgumentNullException("buffer");
            
            if (string.IsNullOrEmpty(pattern))
                throw new ArgumentNullException("pattern");
            
            char last = pattern[pattern.Length - 1];

            for (var i = buffer.Length - 1; i >= startIndex; i--)
            {
                if (buffer[i] == last)
                {
                    bool found = true;

                    for (int j = 1, k = pattern.Length - 2; j < pattern.Length; j++, k--)
                        if (buffer[i - j] != pattern[k])
                        {
                            found = false;
                            break;
                        }

                    if (found) 
                        return i + 1;
                }
            }

            return -1;
        }

        public string Read()
        {
            const string ending = "\r\n\r\n";

            var start = DateTime.Now;

            while (connection.IsClientConnected && !socket.Poll(0, SelectMode.SelectError))
            {
                var time = (DateTime.Now - start);
                if (time.TotalSeconds > 60)
                {
                    connection.Server.LogMessage("Connection killed because of timeout");
                    break;
                }

                if (!socket.HasIncomingData(true)) continue;
                
                if (socket.Available == 0) break;
                
                var read = socket.Receive(portion, portion.Length, SocketFlags.Peek);

                var message = Encoding.ASCII.GetString(portion, 0, read);

                var initialLength = buffer.Length;

                buffer.Append(message);

                var endingPos = FindSubstringEnd(buffer, ending, initialLength);
                if (endingPos >= 0)
                {
                    // ending found
                    read = endingPos - initialLength;
                }

                var newRead = socket.Receive(portion, read, SocketFlags.None);
#if DEBUG
                if (newRead != read) 
                    throw new InvalidDataException("newRead != read");
#endif
                if (endingPos >= 0)
                {
                    message = buffer.Remove(endingPos, buffer.Length - endingPos).ToString();
                    buffer.Remove(0, buffer.Length);
                    return message;
                }
            }

            return null;
        }

    }
}
