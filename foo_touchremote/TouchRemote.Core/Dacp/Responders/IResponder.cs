using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;

namespace TouchRemote.Core.Dacp.Responders
{
    /// <summary>
    /// Basic interface for responders
    /// </summary>
    public interface IResponder
    {
        /// <summary>
        /// Checks if responder is valid (e.g. session exists and so on)
        /// </summary>
        bool IsValid { get; }

        /// <summary>
        /// Generates a HTTP reply for request specified
        /// </summary>
        /// <returns>Reply, or <value>null</value> if request wasn't processed</returns>
        HttpResponse GetResponse();
    }
}
