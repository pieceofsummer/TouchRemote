using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace TouchRemote.Core.Pairing
{
    public interface IClientDevice
    {
        string Name { get; }
        string DeviceId { get; }
        IPEndPoint Host { get; }
        string PairCode { get; }
    }
}
