using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface ILogger
    {

        void LogMessage(string message, params object[] args);
        void LogError(Exception error);

    }
}
