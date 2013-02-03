using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Security.Permissions;

namespace TouchRemote.Bonjour
{
    [SecurityPermission(SecurityAction.InheritanceDemand, UnmanagedCode = true)] 
    [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)] 
    public class DnsHandle : SafeHandleZeroOrMinusOneIsInvalid
    {

        public DnsHandle() : base(true)
        {
        }

        public DnsHandle(IntPtr handle) : this()
        {
            this.handle = handle;
        }


        protected override bool ReleaseHandle()
        {
            Interop.DNSServiceRefDeallocate(handle);
            return true;
        }
    }
}
