using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Core.Pairing
{
    [Serializable]
    public class PairingException : ApplicationException
    {
        public PairingException() { }
        public PairingException(string message) : base(message) { }
        public PairingException(string message, Exception inner) : base(message, inner) { }
        protected PairingException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context)
            : base(info, context) { }
    }
}
