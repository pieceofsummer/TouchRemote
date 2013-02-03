using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Base implementation of <see cref="IResponder"/>
    /// </summary>
    internal abstract class Responder : IResponder
    {

        protected Responder(HttpRequest request)
        {
            Request = request;
        }

        /// <summary>
        /// Returns HTTP request associated with this responder
        /// </summary>
        protected HttpRequest Request { get; private set; }

        /// <summary>
        /// Returns player context
        /// </summary>
        protected IPlayer Player 
        { 
            get 
            {
                if (Request == null)
                    throw new InvalidOperationException("Request was not set");
                var dacpServer = Request.Server as Dacp.DacpServer;
                if (dacpServer == null)
                    throw new InvalidOperationException("Request has no DACP server context");

                return dacpServer.Player; 
            } 
        }

        public abstract HttpResponse GetResponse();

        public virtual bool IsValid { get { return true; } }

    }
}
