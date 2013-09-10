using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using TouchRemote.Core.Http;
using System.Net.NetworkInformation;

namespace TouchRemote.Core.Dacp.Responders
{

    /// <summary>
    /// Answers the /server-info requests
    /// </summary>
    internal class ServerInfoResponder : Responder
    {

        public ServerInfoResponder(HttpRequest request) : base(request)
        {

        }

        public override HttpResponse GetResponse()
        {
            var ifinfo = new MultiValueTag();

            try
            {
                var interfaces = NetworkInterface.GetAllNetworkInterfaces();
                foreach (var iface in interfaces.Where(
                    x => x.OperationalStatus == OperationalStatus.Up &&
                    (x.NetworkInterfaceType == NetworkInterfaceType.Wireless80211 ||
                     x.NetworkInterfaceType == NetworkInterfaceType.Ethernet ||
                     x.NetworkInterfaceType == NetworkInterfaceType.GigabitEthernet)))
                {
                    var addr = iface.GetPhysicalAddress().GetAddressBytes();

                    var bytes = new byte[8];
                    Buffer.BlockCopy(addr, 0, bytes, 0, addr.Length);

                    ifinfo.Add("msma", BitConverter.ToUInt64(bytes, 0));
                }
            }
            catch (Exception ex)
            {
                Player.Logger.LogMessage("Failed to enumerate network interfaces, ignoring");
                ifinfo = null;
            }
            
            return new DmapResponse(new
            {
                msrv = new
                {
                    mstt = 200,
                    mpro = 0x00020009,      // DACP version
                    minm = Player.Name,     // name
                    apro = 0x0003000b,      // DAAP version
                    ppro = 0x00020000,      // unknown
                    aeSV = 0x00030009,      // HomeSharing version
                    ated = (short)7,        // unknown
                    asgr = (short)3,        // unknown, requred to request artists as groups
                    asse = 1L,              // unknown
                    aeMQ = true,            // unknown
                    aeFR = (byte)0x64,      // unknown
                    aeTr = true,            // unknown
                    aeSL = true,            // unknown
                    aeSR = true,            // unknown
                    aeFP = (byte)0,         // FairPlay version
                    //aeSX = 0x6FL,           // unknown, causes FairPlay request
                    msed = true,            // editing supported
                    ceWM = "",              // unknown
                    ceVO = false,           // unknown, probably voice-over?
                    msml = ifinfo,          // network interfaces

                    mslr = true,            // login required
                    msal = true,            // auto logout
                    mstm = 1800,            // auto logout timeout
                    msas = (byte)0x3,       // authentication schemas
                    msup = true,            // supports 'update' queries
                    mspi = true,            // supports persistent identifiers
                    msex = true,            // supports extensions (?)
                    msbr = true,            // supports 'browse' queries
                    msqy = true,            // supports query (?)
                    msix = true,            // supports index (?)
                    msrs = true,            // supports resolve (?)

                    msdc = 1,               // database count

                    mstc = DateTime.UtcNow.ToUnixTime(),    // UTC time
                    msto = (int)Math.Round(DateTimeOffset.Now.Offset.TotalSeconds)  // UTC offset
                }
            });
        }

    }
}
