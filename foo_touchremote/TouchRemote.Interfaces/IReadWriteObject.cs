using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IReadWriteObject
    {

        IDisposable BeginRead();
        IDisposable BeginWrite();

    }
}
