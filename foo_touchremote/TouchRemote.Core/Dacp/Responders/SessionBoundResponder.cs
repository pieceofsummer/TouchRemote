using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using TouchRemote.Interfaces;
using System.Runtime.CompilerServices;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Base implementation of <see cref="IResponder"/> for session-bound requests
    /// </summary>
    internal abstract class SessionBoundResponder : Responder
    {

        /// <summary>
        /// Returns identifier of the session for current request
        /// </summary>
        protected int SessionId { get; private set; }

        protected SessionBoundResponder(HttpRequest request) : base(request)
        {
            var sessionIdStr = request.QueryString["session-id"];
            if (string.IsNullOrEmpty(sessionIdStr))
                throw new ArgumentException("Session ID is required for this type of request", "request");

            SessionId = int.Parse(sessionIdStr);

            Session = SessionManager.GetSession(SessionId);
        }

        public override bool IsValid { get { return Session != null; } }

        /// <summary>
        /// Returns <see cref="Session"/> object for current request
        /// </summary>
        protected Session Session { get; private set; }

    }
}
