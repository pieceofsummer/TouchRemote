using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Core.Http.Response
{
    public sealed class InvalidRequestResponse : TextHttpResponse
    {

        public string Message { get; private set; }

        public InvalidRequestResponse(string message)
        {
            Code = 400;
            Reason = "Invalid Request";
            Message = (string.IsNullOrEmpty(message)) ? Reason : message;
        }

        protected override string GetText()
        {
            return Message;
        }

    }
}
