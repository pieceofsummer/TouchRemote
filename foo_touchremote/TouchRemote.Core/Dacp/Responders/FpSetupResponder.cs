using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Net;
using System.IO;
using System.Security.Cryptography;

namespace TouchRemote.Core.Dacp.Responders
{
    /// <summary>
    /// reserved for the future
    /// </summary>
    internal class FpSetupResponder : SessionBoundResponder
    {

        public FpSetupResponder(HttpRequest request) : base(request)
        {
            if (request.Method != "POST")
                throw new InvalidOperationException("fp-setup must be a POST request");
        }

        public override HttpResponse GetResponse()
        {
            var data = Request.PostData;

            // Data comes as: 
            // > FPLY 64 01 01 00 length(0x04) 02 00 XX BB
            // < FPLY 64 01 02 00 length(0x82) 02 XX (128 other bytes)
            // > FPLY 64 01 03 00 length(0x98) XX 8F 1A 9C (148 other bytes)
            // < FPLY 64 01 04 00 length(0x14) (20 bytes)

            if (data.Length < 12)
                throw new InvalidOperationException("Data is too short");

            if (Encoding.ASCII.GetString(data, 0, 4) != "FPLY")
                throw new InvalidOperationException("Invalid data signature");

            if (data[4] != 0x64 || data[5] != 0x01 || (data[6] & 1) == 0 || data[7] != 0)
                throw new InvalidOperationException("Invalid data version: " + BitConverter.ToUInt32(data, 4).ToString("x8"));

            var payloadSize = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(data, 8));

            Player.Logger.LogMessage("got request: {0}", BitConverter.ToString(data));

            if (data[6] == 1)
            {
                // first one
                if (payloadSize != 4)
                    throw new InvalidOperationException("Invalid data size");

                var payload = new byte[130];
                payload[0] = data[12];
                payload[1] = data[14];

                return new FpResponse(data[4], data[5], 2, 0, payload);
            }
            else if (data[6] == 3)
            {
                // second one

                if (payloadSize != 152)
                    throw new InvalidOperationException("Invalid data size");

                var payload = SHA1.Create().ComputeHash(data, 16, payloadSize - 4);

                return new FpResponse(data[4], data[5], 4, 0, payload);
            }
            else
                throw new InvalidOperationException("Invalid request sequence: " + data[6]);
        }

    }
}
