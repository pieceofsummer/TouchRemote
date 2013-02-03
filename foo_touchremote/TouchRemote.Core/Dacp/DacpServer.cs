using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using TouchRemote.Core.Http;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Dacp
{
    public class DacpServer : HttpServer
    {
        public IPlayer Player { get; private set; }

        public DacpServer(IPlayer player, ushort port) : base(port)
        {
            if (player == null)
                throw new ArgumentNullException("player");

            Player = player;
        }

        protected internal override void LogMessage(string format, params object[] args)
        {
            Player.Logger.LogMessage(format, args);
        }

    }
}