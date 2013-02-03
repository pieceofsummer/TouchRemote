using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Core.Http.Response
{
    public sealed class NoContentResponse : HttpResponse
    {

        public NoContentResponse()
        {
            Code = 204;
            Reason = "No Content";
        }

        protected override byte[] GetData()
        {
            return null;
        }

    }
}
