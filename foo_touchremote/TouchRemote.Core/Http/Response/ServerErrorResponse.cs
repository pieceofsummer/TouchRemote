using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Core.Http.Response
{
    public sealed class ServerErrorResponse : TextHttpResponse
    {
        public string Data { get; private set; }
        
        public ServerErrorResponse(Exception ex)
        {
            Code = 500;
            Reason = "Internal Server Error";
            Data = ex.ToString();
        }

        protected override string GetText()
        {
            return Data;
        }
    }
}
