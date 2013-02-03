using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Pairing;

namespace TouchRemote.Bonjour
{
    internal class BonjourDevice : IClientDevice
    {
        public string Name { get; set; }

        public string DeviceId { get; set; }

        public System.Net.IPEndPoint Host { get; set; }

        public string PairCode { get; set; }

        internal bool IsComplete { get; set; }

        internal DnsServiceErrorType Error { get; set; }
    }
}
