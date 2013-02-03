using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace TouchRemote.Core.Pairing
{
    internal class PairingReply
    {

        public PairingReply(byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException("data");
            if (data.Length < 24)
                throw new ArgumentException("Data is too short", "data");

            var rootName = Encoding.ASCII.GetString(data, 0, 4);
            if (rootName != "cmpa")
                throw new ArgumentException("Invalid magic", "data");

            var rootLength = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(data, 4));
            if (rootLength != data.Length - 8)
                throw new ArgumentException("Invalid length", "data");

            for (int offset = 8; offset < data.Length; )
            {
                var name = Encoding.ASCII.GetString(data, offset, 4);
                var length = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(data, offset + 4));

                switch (name)
                {
                    case "cmpg":
                        if (length != 8)
                            throw new ArgumentException("Invalid pairing guid length", "data");
                        
                        PairingGuid = new byte[length];
                        Buffer.BlockCopy(data, offset + 8, PairingGuid, 0, length);
                        break;

                    case "cmnm":
                        Name = Encoding.UTF8.GetString(data, offset + 8, length);
                        break;

                    case "cmty":
                        Type = Encoding.UTF8.GetString(data, offset + 8, length);
                        break;
                }

                offset += length + 8;
            }

            if (PairingGuid == null)
                throw new ArgumentException("No pairing guid in reply", "data");

            if (string.IsNullOrEmpty(Name))
                throw new ArgumentException("No device name in reply", "data");
        }

        public byte[] PairingGuid { get; private set; }
        public string Name { get; private set; }
        public string Type { get; private set; }

    }
}
