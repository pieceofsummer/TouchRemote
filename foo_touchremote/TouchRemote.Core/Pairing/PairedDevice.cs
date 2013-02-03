using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32;

namespace TouchRemote.Core.Pairing
{
    public class PairedDevice
    {
        public string Name { get; internal set; }
        public string DeviceId { get; internal set; }
        public ulong PairingGuid { get; internal set; }

        private PairedDevice()
        {
        }

        internal static PairedDevice Read(RegistryKey k, string id)
        {
            if (k == null)
                throw new ArgumentNullException("k");
            if (string.IsNullOrEmpty(id))
                throw new ArgumentNullException("id");

            var guid = k.GetValue("Cookie") as byte[];
            if (guid == null || guid.Length != 8) return null;

            var name = k.GetValue(null) as string;
            if (string.IsNullOrEmpty(name)) return null;                

            return new PairedDevice
            {
                Name = name,
                DeviceId = id,
                PairingGuid = BitConverter.ToUInt64(guid, 0)
            };
        }
    }
}
