using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Net.Sockets;
using System.Threading;

namespace TouchRemote.Bonjour
{

    internal class Interop
    {
        private const string DnsSd = "dnssd.dll";

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern void DNSServiceRefDeallocate(IntPtr handle);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern int DNSServiceRefSockFD(IntPtr handle);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern DnsServiceErrorType DNSServiceProcessResult(IntPtr handle);
        
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate void DNSServiceResolveReply(
            IntPtr handle, DnsServiceFlags flags, 
            int ifIndex, DnsServiceErrorType errorCode,
            string fullName, string hostTarget, ushort port,
            ushort txtLen, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)] byte[] txtRecord, 
            IntPtr context);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern DnsServiceErrorType DNSServiceResolve(
            out IntPtr handle, DnsServiceFlags flags, 
            int ifIndex, string name, string regType, string domain,
            DNSServiceResolveReply callback, IntPtr context);

        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate void DNSServiceBrowseReply(
            IntPtr handle, DnsServiceFlags flags, 
            int ifIndex, DnsServiceErrorType errorCode,
            string serviceName, string regType, string domain,
            IntPtr context);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern DnsServiceErrorType DNSServiceBrowse(
            out IntPtr handle, DnsServiceFlags flags,
            int ifIndex, string regType, string domain,
            DNSServiceBrowseReply callback, IntPtr context);

        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate void DnsServiceQueryRecordReply(
            IntPtr handle, DnsServiceFlags flags, int ifIndex, DnsServiceErrorType errorCode, string fullName, 
            [MarshalAs(UnmanagedType.U2)] DnsServiceType rrType, [MarshalAs(UnmanagedType.U2)] DnsServiceClass rrClass, 
            ushort rdLen, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 7)] byte[] rData,
            uint ttl, IntPtr context);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern DnsServiceErrorType DNSServiceQueryRecord(
            out IntPtr handle, DnsServiceFlags flags, int ifIndex, string fullName, 
            [MarshalAs(UnmanagedType.U2)] DnsServiceType rrType, [MarshalAs(UnmanagedType.U2)] DnsServiceClass rrClass,
            DnsServiceQueryRecordReply callback, IntPtr context);

        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate void DnsServiceRegisterReply(
            IntPtr handle, DnsServiceFlags flags, DnsServiceErrorType errorCode,
            string name, string regType, string domain, 
            IntPtr context);

        [DllImport(DnsSd, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern DnsServiceErrorType DNSServiceRegister(
            out IntPtr handle, DnsServiceFlags flags,
            int ifIndex, string name, string regType, string domain, 
            string host, ushort port,
            ushort txtLen, [MarshalAs(UnmanagedType.LPArray)] byte[] txtRecord,
            DnsServiceRegisterReply callback, IntPtr context);

        [DllImport("ws2_32.dll", CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Auto)]
        private static extern int select(int nsock, [In, Out] FdSet read, [In, Out] FdSet write, [In, Out] FdSet error, TimeVal tv);

        [StructLayout(LayoutKind.Sequential)]
        private class TimeVal
        {
            
            public TimeVal(TimeSpan value)
            {
                sec = (int)Math.Floor(value.TotalSeconds);
                usec = value.Milliseconds * 1000;
            }

            public int sec;
            public int usec;
        }

        [StructLayout(LayoutKind.Sequential)]
        private class FdSet
        {
            public FdSet()
            {
                count = 0;
                sockets = new int[64];
            }

            public FdSet(int socket) : this()
            {
                Add(socket);
            }

            public void Add(int socket)
            {
                sockets[count++] = socket;
            }

            public int count;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)] public int[] sockets;
        }

        public static bool DNSServiceHasData(IntPtr handle, TimeSpan? waitFor)
        {
            var sock = DNSServiceRefSockFD(handle);

            var read = new FdSet(sock);
            var error = new FdSet(sock);
            var tv = (waitFor.HasValue) ? new TimeVal(waitFor.Value) : null;

            var r = select(1, read, null, error, tv);

            return (r > 0);
        }

    }

}
