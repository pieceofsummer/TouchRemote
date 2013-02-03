using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Bonjour
{

    internal enum DnsServiceErrorType : int
    {
        NoError = 0,
        Unknown = -65537,       /* 0xFFFE FFFF */
        NoSuchName = -65538,
        NoMemory = -65539,
        BadParam = -65540,
        BadReference = -65541,
        BadState = -65542,
        BadFlags = -65543,
        Unsupported = -65544,
        NotInitialized = -65545,
        AlreadyRegistered = -65547,
        NameConflict = -65548,
        Invalid = -65549,
        Firewall = -65550,
        Incompatible = -65551,  /* client library incompatible with daemon */
        BadInterfaceIndex = -65552,
        Refused = -65553,
        NoSuchRecord = -65554,
        NoAuth = -65555,
        NoSuchKey = -65556,
        NATTraversal = -65557,
        DoubleNAT = -65558,
        BadTime = -65559,
        /* mDNS Error codes are in the range FFFE FF00 (-65792) to FFFE FFFF (-65537) */
    }

}
