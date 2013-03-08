using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Diagnostics;
using System.Net;
using Microsoft.Win32;
using System.Web;

namespace TouchRemote.Core.Pairing
{

    /// <summary>
    /// uses registry for testing purposes, too lazy to bother with files
    /// </summary>
    public static class PairedDevices
    {
        
        public static void PerformPairing(this IClientDevice device, string pinCode, string serviceName)
        {
            if (device == null)
                throw new ArgumentNullException("device");
            if (string.IsNullOrEmpty(pinCode))
                throw new ArgumentNullException("pinCode");
            if (pinCode.Length != 4 || !pinCode.All(char.IsDigit))
                throw new ArgumentException("PIN code must be exactly 4 digits", "pinCode");
            if (string.IsNullOrEmpty(serviceName))
                throw new ArgumentNullException("serviceName");

            Debug.Print("Trying to pair with device {0} at {1}", device.Name, device.Host);

            string pairCode = device.PairCode;
            if (string.IsNullOrEmpty(pairCode) || pairCode.Length != 16 ||
                !pairCode.All(c => char.IsDigit(c) || (c >= 'A' && c <= 'F')))
            {
                // PairCode should be exactly 16 uppercase hexadecimal chars
                throw new PairingException("Device didn't return a valid pair code");
            }

            // here some magic goes ;)

            byte[] data = new byte[24];
            Encoding.ASCII.GetBytes(pairCode, 0, 16, data, 0);
            Encoding.Unicode.GetBytes(pinCode, 0, 4, data, 16);

            data = new Delay.MD5Managed().ComputeHash(data);

            var hashString = new StringBuilder(32);
            for (int i = 0; i < data.Length; i++)
                hashString.AppendFormat("{0:X2}", data[i]);

            string url = string.Format("http://{0}/pair?pairingcode={1}&servicename={2}", device.Host, hashString, HttpUtility.UrlPathEncode(serviceName));

            using (WebClient client = new WebClient())
            {
                try
                {
                    data = client.DownloadData(url);
                }
                catch (WebException ex)
                {
                    if (ex.Status == WebExceptionStatus.ProtocolError)
                        if (((HttpWebResponse)ex.Response).StatusCode == HttpStatusCode.NotFound)
                            throw new PairingException("Pin code is invalid");

                    throw new PairingException("Unable to complete pairing: " + ex.Message);
                }
            }

            PairingReply reply;
            try
            {
                reply = new PairingReply(data);
            }
            catch (ArgumentException ex) // PairingReply constructor doesn't throw any other exception types
            {
                throw new PairingException("Unexpected reply from device", ex);
            }

            var key = Registry.CurrentUser.CreateSubKey(@"Software\Wintense\TouchRemote\Devices\" + device.DeviceId);
            key.SetValue(null, reply.Name);
            key.SetValue("Cookie", reply.PairingGuid, RegistryValueKind.Binary);
            key.SetValue("Type", reply.Type, RegistryValueKind.String);
            key.Close();

            cachedDevices = null;
        }
        
        public static PairedDevice GetDevice(ulong pairingGuid)
        {
            return List.FirstOrDefault(x => x.PairingGuid == pairingGuid);
        }

        public static PairedDevice GetDevice(string deviceId)
        {
            return List.FirstOrDefault(x => string.Equals(x.DeviceId, deviceId, StringComparison.OrdinalIgnoreCase));
        }

        public static void ForgetDevice(string deviceId)
        {
            if (string.IsNullOrEmpty(deviceId))
                throw new ArgumentNullException("deviceId");

            Registry.CurrentUser.DeleteSubKey(@"Software\Wintense\TouchRemote\Devices\" + deviceId);
            cachedDevices = null;
        }

        public static void ForgetDevice(this PairedDevice device)
        {
            if (device == null)
                throw new ArgumentNullException("device");
            ForgetDevice(device.DeviceId);
        }

        private static IList<PairedDevice> cachedDevices = null;

        public static IEnumerable<PairedDevice> List
        {
            get
            {
                if (cachedDevices != null)
                    return cachedDevices;

                var devs = new List<PairedDevice>();
                var key = Registry.CurrentUser.CreateSubKey(@"Software\Wintense\TouchRemote\Devices");
                foreach (var devId in key.GetSubKeyNames())
                {
                    var devKey = key.OpenSubKey(devId);
                    if (devKey == null) continue;

                    var dev = PairedDevice.Read(devKey, devId);
                    if (dev != null)
                        devs.Add(dev);

                    devKey.Close();
                }
                key.Close();

                return (cachedDevices = devs);
            }
        }

    }
}
