using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Bonjour;
using System.Collections.Specialized;
using TouchRemote.Core.Raop;
using TouchRemote.Core;
using TouchRemote.Core.Http;
using TouchRemote.Core.Http.Response;
using TouchRemote.Core.Raop.Response;

namespace TouchRemote.Raop.Test
{
    class Program
    {
        private static HttpResponse Handler(HttpRequest request)
        {
            Console.WriteLine("TRACE: {0} {1} {2}/{3}", request.Method, request.Path, request.Protocol, request.Version);
            foreach (string v in request.Headers)
                Console.WriteLine("  {0}: {1}", v, request.Headers[v]);
            Console.WriteLine();
            
            switch (request.Method)
            {
                case "OPTIONS":
                    return new OptionsResponse(request);

                case "ANNOUNCE":
                    return new AnnounceResponse(request);

                case "SETUP":
                    return new SetupResponse(request);

                default:
                    return new NotFoundResponse();
            }
        }
        
        static void Main(string[] args)
        {
            const ushort port = 5000;

            var bonjourService = new BonjourService();
            
            bonjourService.Start("00DEADBEEF10@MyPort", "_raop._tcp", "local.", null, port, new NameValueCollection
            {
                { "txtvers", "1" },
                { "tp", "UDP" },    // protocol
                { "sm", "false" },
                { "sv", "false" },
                { "ek", "1" },
                { "et", "0,1" },
                { "cn", "0,1" },
                { "ch", "2" },      // channels
                { "ss", "16" },     // bits per sample
                { "sr", "44100" },  // sample rate
                { "pw", "false" },  // password-protected
                { "vn", "3" }       // version number (?)
            });

            var raopServer = new RaopServer(port);
            raopServer.RequestHandler = Handler;
            raopServer.Start();

            Console.WriteLine("Working...");
            Console.ReadLine();

            raopServer.Stop();
            bonjourService.Stop();
        }
    }
}
