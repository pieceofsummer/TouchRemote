using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http.Response;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Answers the /logout requests
    /// </summary>
    internal class LogoutResponder : SessionBoundResponder
    {

        public LogoutResponder(HttpRequest request) : base(request)
        {
        }

        public override HttpResponse GetResponse()
        {
            SessionManager.TerminateSession(SessionId);
            return new NoContentResponse();
        }
    }
}
