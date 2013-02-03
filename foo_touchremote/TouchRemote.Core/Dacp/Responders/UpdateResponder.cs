using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Threading;

namespace TouchRemote.Core.Dacp.Responders
{
    internal class UpdateResponder : SessionBoundResponder
    {
        public UpdateResponder(HttpRequest request) : base(request) { }

        public override HttpResponse GetResponse()
        {
            var revisionStr = Request.QueryString["revision-number"];
            uint revisionNumber = 0;
            if (!string.IsNullOrEmpty(revisionStr) && uint.TryParse(revisionStr, out revisionNumber))
            {
                if (Session.DatabaseRevision > 0)
                {
                    while (revisionNumber > Session.DatabaseRevision)
                    {
                        Thread.Sleep(50);
                        if (!Request.IsClientConnected)
                        {
                            return new DmapResponse(new
                            {
                                mupd = new
                                {
                                    mstt = 200,
                                    musr = 0
                                }
                            });
                        }
                    }
                }
                else
                    Session.DatabaseRevision = 1;
            }

            return new DmapResponse(new
            {
                mupd = new
                {
                    mstt = 200,
                    musr = Session.DatabaseRevision + 1
                }
            });
        }
    }
}
