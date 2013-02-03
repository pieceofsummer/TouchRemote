using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using TouchRemote.Core.Http;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Http
{
    public class HttpServer
    {
        private readonly Socket listener, listener6;
        private readonly ManualResetEvent exiting;
        private Thread serverThread;

        public HandleRequestDelegate RequestHandler { get; set; }

        public ushort Port { get; private set; }

        public bool IsRunning { get; private set; }

        public HttpServer(ushort port)
        {
            Port = port;
            exiting = new ManualResetEvent(false);
            listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            if (Socket.OSSupportsIPv6)
                listener6 = new Socket(AddressFamily.InterNetworkV6, SocketType.Stream, ProtocolType.Tcp);
            else
                listener6 = null;
        }

        public void Start()
        {
            if (IsRunning) return;

            listener.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.ReuseAddress, true);
            listener.ExclusiveAddressUse = false;
            listener.Bind(new IPEndPoint(IPAddress.Any, Port));
            listener.Listen(30);

            if (listener6 != null)
            {
                listener6.SetSocketOption(SocketOptionLevel.IPv6, SocketOptionName.ReuseAddress, true);
                listener6.ExclusiveAddressUse = false;
                listener6.Bind(new IPEndPoint(IPAddress.IPv6Any, Port));
                listener6.Listen(30);
            }

            exiting.Reset();

            serverThread = new Thread(ListenerThread);
            serverThread.Start();

            IsRunning = true;
        }
        
        public void Stop()
        {
            exiting.Set();

            if (serverThread != null)
            {
                serverThread.Join();
                serverThread = null;
            }

            IsRunning = false;
        }

        private void ListenerThread()
        {
            Console.WriteLine("Listener thread started");

            while (!IsStopping)
            {
                if (listener.HasIncomingData(false))
                    listener.BeginAccept(IncomingConnection, this);
                if (listener6 != null && listener6.HasIncomingData(false))
                    listener6.BeginAccept(IncomingConnection6, this);
            }

            listener.Close();

            if (listener6 != null)
                listener6.Close();

            Console.WriteLine("Listener thread terminated");
        }

        internal bool IsStopping { get { return exiting.WaitOne(10); } }

        private static void IncomingConnection(IAsyncResult ar)
        {
            var server = (HttpServer)ar.AsyncState;

            var client = server.listener.EndAccept(ar);
            
            ThreadPool.QueueUserWorkItem(WorkerConnection, new HttpConnection(client, server));
        }

        private static void IncomingConnection6(IAsyncResult ar)
        {
            var server = (HttpServer)ar.AsyncState;

            var client = server.listener6.EndAccept(ar);

            ThreadPool.QueueUserWorkItem(WorkerConnection, new HttpConnection(client, server));
        }

        private static long connectionCount = 0;

        private static void WorkerConnection(object state)
        {
            var connection = (HttpConnection)state;

            var count = Interlocked.Increment(ref connectionCount);
#if DEBUG
            connection.Server.Player.Logger.LogMessage("Connection++, count: {0}", count);
#endif
            try
            {
                connection.Work();
            }
            catch (Exception ex)
            {
                connection.Server.LogError(ex);
            }
            finally
            {
                count = Interlocked.Decrement(ref connectionCount);
#if DEBUG
                connection.Server.Player.Logger.LogMessage("Connection--, count: {0}", count);
#endif
                GC.Collect(1);
            }
        }

        public void WaitForConnectionsClosed(TimeSpan waitFor)
        {
            TimeSpan waited = TimeSpan.Zero;
            TimeSpan delta = TimeSpan.FromMilliseconds(100);

            while (connectionCount > 0)
            {
                Thread.Sleep(delta);
                waited += delta;
                //if (waited >= waitFor) break;
                break;
            }
        }

        protected internal virtual bool IsValidProtocol(string protocol, string version)
        {
            if (!string.Equals(protocol, "HTTP", StringComparison.OrdinalIgnoreCase))
                return false;

            return version == "1.0" || version == "1.1";
        }

        protected internal virtual bool? HasPostData(string method)
        {
            if (string.Equals(method, "GET", StringComparison.OrdinalIgnoreCase)) return false;
            if (string.Equals(method, "POST", StringComparison.OrdinalIgnoreCase)) return true;
            return null;
        }

        protected internal virtual void LogError(Exception error)
        {
            if (error == null) return;

            Console.WriteLine(error.ToString());
        }

        protected internal virtual void LogMessage(string format, params object[] args)
        {
            Console.WriteLine(format, args);
        }

    }
}