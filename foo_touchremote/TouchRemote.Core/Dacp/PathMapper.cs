using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Dacp.Responders;
using TouchRemote.Core.Http;
using System.Text.RegularExpressions;

namespace TouchRemote.Core.Dacp
{
    public static class PathMapper
    {

        public static IResponder Map(HttpRequest request)
        {
            if (request == null) return null;

            if (request.Path == "/server-info")
            {
                return new ServerInfoResponder(request);
            }
            else if (request.Path == "/login")
            {
                return new LoginResponder(request);
            }
            else if (request.Path == "/logout")
            {
                return new LogoutResponder(request);
            }
            else if (request.Path == "/fp-setup")
            {
                return new FpSetupResponder(request);
            }
            else if (request.Path == "/update")
            {
                return new UpdateResponder(request);
            }
            else if (request.Path == "/databases")
            {
                return new DatabasesResponder(request);
            }
            else if (request.Path.StartsWith("/databases/"))
            {
                return new DatabaseInstanceResponder(request);
            }
            else if (request.Path == "/ctrl-int")
            {
                return new CtrlIntResponder(request);
            }
            else if (request.Path.StartsWith("/ctrl-int/"))
            {
                return new CtrlIntInstanceResponder(request);
            }

            // no handler :(
            return null;
        }

    }
}
