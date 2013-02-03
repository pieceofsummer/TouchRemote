using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Core.Http.Response
{
    public sealed class NotFoundResponse : TextHttpResponse
    {

        public NotFoundResponse()
        {
            Code = 404;
            Reason = "Not Found";
        }

        protected override string GetText()
        {
            return Reason;
        }

    }
}
