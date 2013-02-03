using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Dacp.Responders
{
    internal class DatabasesResponder : SessionBoundResponder
    {

        public DatabasesResponder(HttpRequest request) : base(request)
        {

        }

        public override HttpResponse GetResponse()
        {
            return new DmapResponse(new
            {
                avdb = new
                {
                    mstt = 200,
                    muty = (byte)0,
                    mlcl = new[]
                    {
                        new 
                        { 
                            miid = 1,
                            mdbk = 1,                   // database kind?
                            minm = Player.Name,
                            mper = Player.DatabaseId,
                            aeMk = 3,
                            meds = 3                    // edit capabilities
                        }
                    }
                }
            });
        }
    }
}
