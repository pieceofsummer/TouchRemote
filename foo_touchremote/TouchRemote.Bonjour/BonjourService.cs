using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Specialized;
using System.Net;

namespace TouchRemote.Bonjour
{

    public class BonjourService
    {
        private readonly Dictionary<string, IntPtr> m_handles = new Dictionary<string, IntPtr>(StringComparer.OrdinalIgnoreCase);

        private readonly Interop.DnsServiceRegisterReply registerCallback;

        public BonjourService()
        {
            registerCallback = new Interop.DnsServiceRegisterReply(RegisterCallback);
        }

        public void Start(string serviceName, string regType, string domain, string host, ushort port, NameValueCollection txt)
        {
            string key = serviceName.Trim('.') + "." + regType.Trim('.') + "." + domain.Trim('.');

            if (m_handles.ContainsKey(key))
                throw new InvalidOperationException("Service \"" + key + "\" is already registered");

            ushort txtLen = 0;
            byte[] txtRecord = null;
            if (txt != null && txt.Count > 0)
            {
                txtRecord = txt.ToArray();
                if (txtRecord.Length > ushort.MaxValue)
                    throw new ArgumentException("TXT data is too large", "txt");
                txtLen = (ushort)txtRecord.Length;
            }

            var netPort = (ushort)IPAddress.HostToNetworkOrder((short)port);

            IntPtr handle;

            var r = Interop.DNSServiceRegister(out handle, DnsServiceFlags.Default | DnsServiceFlags.AllowRemoteQuery, 0, serviceName, regType, domain, host, netPort, txtLen, txtRecord, registerCallback, IntPtr.Zero);

            r.ThrowException("DNSServiceRegister");

            r = Interop.DNSServiceProcessResult(handle);

            r.ThrowException("DNSServiceProcessResult");
        }

        public void Stop(string service)
        {
            lock (m_handles)
            {
                IntPtr handle;
                if (m_handles.TryGetValue(service, out handle))
                {
                    Interop.DNSServiceRefDeallocate(handle);
                    m_handles.Remove(service);
                }
            }
        }

        public void Stop()
        {
            lock (m_handles)
            {
                foreach (var pair in m_handles)
                {
                    Interop.DNSServiceRefDeallocate(pair.Value);
                }
                m_handles.Clear();
            }
        }

        private void RegisterCallback(IntPtr handle, DnsServiceFlags flags, DnsServiceErrorType errorCode, string name, string regType, string domain, IntPtr context)
        {
            Console.WriteLine("Register result: " + errorCode);

            if (errorCode == DnsServiceErrorType.NoError)
            {
                string key = name.Trim('.') + "." + regType.Trim('.') + "." + domain.Trim('.');

                lock (m_handles)
                    m_handles[key] = handle;
            }
        }

    }

}
