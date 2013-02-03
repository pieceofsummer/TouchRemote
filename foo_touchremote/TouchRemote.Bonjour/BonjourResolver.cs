using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Pairing;
using System.Net;
using System.Collections.Specialized;

namespace TouchRemote.Bonjour
{

    internal static class Util
    {
        public static void ThrowException(this DnsServiceErrorType error, string context)
        {
            if (error == DnsServiceErrorType.NoError) return;

            throw new Exception("Bonjour error from " + context + ": " + error);
        }
    }

    public class BonjourResolver
    {
        private readonly Interop.DNSServiceBrowseReply browseCallback;
        private readonly Interop.DNSServiceResolveReply resolveCallback;
        private readonly Interop.DnsServiceQueryRecordReply queryPtrCallback;

        public BonjourResolver()
        {
            browseCallback = new Interop.DNSServiceBrowseReply(BrowseCallback);
            resolveCallback = new Interop.DNSServiceResolveReply(ResolveCallback);
            queryPtrCallback = new Interop.DnsServiceQueryRecordReply(QueryPtrCallback);
        }

        private static readonly Dictionary<IntPtr, Dictionary<string, BonjourDevice>> lookups = new Dictionary<IntPtr, Dictionary<string, BonjourDevice>>();

        private volatile bool isCancelled = false;

        public IList<IClientDevice> FindDevices(string deviceType, string domain)
        {
            return FindDevicesImpl(deviceType, domain, null);
        }

        public IList<IClientDevice> FindDevices(string deviceType, string domain, TimeSpan timeout)
        {
            return FindDevicesImpl(deviceType, domain, timeout);
        }

        public void CancelFind()
        {
            isCancelled = true;
        }

        private IList<IClientDevice> FindDevicesImpl(string deviceType, string domain, TimeSpan? timeout)
        {
            IntPtr handle;
            var r = Interop.DNSServiceBrowse(out handle, DnsServiceFlags.Default | DnsServiceFlags.ForceMulticast, 0, deviceType, domain, browseCallback, IntPtr.Zero);
            
            r.ThrowException("DNSServiceBrowse");

            var deviceList = new Dictionary<string, BonjourDevice>(StringComparer.OrdinalIgnoreCase);
                        
            lock (lookups) 
                lookups.Add(handle, deviceList);

            isCancelled = false;

            try
            {
                TimeSpan quantum = TimeSpan.FromSeconds(0.5);
                TimeSpan elapsed = TimeSpan.Zero;

                while (timeout == null || elapsed < timeout.Value)
                {
                    if (Interop.DNSServiceHasData(handle, quantum))
                    {
                        r = Interop.DNSServiceProcessResult(handle);

                        r.ThrowException("DNSServiceProcessResult");

                        break;
                    }

                    elapsed += quantum;

                    if (isCancelled) break;
                }
            }
            finally
            {
                lock (lookups) 
                    lookups.Remove(handle); 
                Interop.DNSServiceRefDeallocate(handle);
            }

            return deviceList.Values.Distinct().Where(x => x.IsComplete).OfType<IClientDevice>().ToList();
        }

        private void BrowseCallback(IntPtr h, DnsServiceFlags flags, int ifIndex, DnsServiceErrorType errorCode, string serviceName, string regType, string domain, IntPtr context)
        {
            errorCode.ThrowException("BrowseCallback");

            Dictionary<string, BonjourDevice> deviceList;
            lock (lookups) 
                if (!lookups.TryGetValue(h, out deviceList)) return;

            var device = new BonjourDevice { DeviceId = serviceName, IsComplete = false, Error = DnsServiceErrorType.NoError };

            lock (deviceList) 
                deviceList[serviceName] = device;
            
            IntPtr handle;
            var r = Interop.DNSServiceResolve(out handle, DnsServiceFlags.Default | DnsServiceFlags.ForceMulticast, ifIndex, serviceName, regType, domain, resolveCallback, h);

            if (r != DnsServiceErrorType.NoError)
            {
                device.Error = r;
                return;
            }

            lock (deviceList) 
                deviceList["0x" + handle.ToString("X")] = device;

            try
            {
                if (!Interop.DNSServiceHasData(handle, TimeSpan.FromSeconds(5))) return;

                r = Interop.DNSServiceProcessResult(handle);

                if (r != DnsServiceErrorType.NoError)
                {
                    device.Error = r;
                    return;
                }
            }
            finally
            {
                lock (deviceList) 
                    deviceList.Remove("0x" + handle.ToString("X"));
                Interop.DNSServiceRefDeallocate(handle);
            }
        }
        
        private void ResolveCallback(IntPtr h, DnsServiceFlags flags, int ifIndex, DnsServiceErrorType errorCode, string fullName, string hostTarget, ushort port, ushort txtLen, byte[] txtRecord, IntPtr context)
        {            
            Dictionary<string, BonjourDevice> deviceList;
            lock (lookups)
                if (!lookups.TryGetValue(context, out deviceList)) return;

            BonjourDevice device;
            lock (deviceList)
                if (!deviceList.TryGetValue("0x" + h.ToString("X"), out device)) return;

            if (errorCode != DnsServiceErrorType.NoError)
            {
                device.Error = errorCode;
                return;
            }          

            var hostPort = (ushort)IPAddress.NetworkToHostOrder((short)port);

            device.Host = new System.Net.IPEndPoint(0, hostPort);
            
            var txt = txtRecord.FromArray();

            device.Name = txt["DvNm"];
            device.PairCode = txt["Pair"];

            IntPtr handle;
            var r = Interop.DNSServiceQueryRecord(out handle, DnsServiceFlags.Default | DnsServiceFlags.ForceMulticast, ifIndex, hostTarget, DnsServiceType.A, DnsServiceClass.IN, queryPtrCallback, context);

            if (r != DnsServiceErrorType.NoError)
            {
                device.Error = errorCode;
                return;
            }

            lock (deviceList)
                deviceList[hostTarget] = device;

            try
            {
                if (!Interop.DNSServiceHasData(handle, TimeSpan.FromSeconds(5))) return;

                r = Interop.DNSServiceProcessResult(handle);

                if (r != DnsServiceErrorType.NoError)
                {
                    device.Error = r;
                }
            }
            finally
            {
                Interop.DNSServiceRefDeallocate(handle);
                lock (deviceList)
                    deviceList.Remove(hostTarget);
            }
        }

        private static void QueryPtrCallback(IntPtr h, DnsServiceFlags flags, int ifIndex, DnsServiceErrorType errorCode, string fullName, DnsServiceType rrType, DnsServiceClass rrClass, ushort dataLen, byte[] data, uint ttl, IntPtr context)
        {            
            Dictionary<string, BonjourDevice> deviceList;
            lock (lookups)
                if (!lookups.TryGetValue(context, out deviceList)) return;

            BonjourDevice device;
            lock (deviceList)
                if (!deviceList.TryGetValue(fullName, out device)) return;

            if (errorCode != DnsServiceErrorType.NoError)
            {
                device.Error = errorCode;
                return;
            }

            device.Host.Address = new IPAddress(data);
            device.IsComplete = true;
        }

    }

}
