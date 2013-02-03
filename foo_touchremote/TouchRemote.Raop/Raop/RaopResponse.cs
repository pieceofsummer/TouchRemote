using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Http;
using System.IO;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Runtime.CompilerServices;
using System.Reflection;
using System.Net.Sockets;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Engines;
using Org.BouncyCastle.Crypto.Encodings;

namespace TouchRemote.Core.Raop
{
    public class RaopResponse : HttpResponse
    {
        protected static AsymmetricCipherKeyPair rsaKey;

        private static AsymmetricCipherKeyPair CreateCryptoKeys()
        {
            var reader = new Org.BouncyCastle.OpenSsl.PemReader(new StringReader(
@"-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA59dE8qLieItsH1WgjrcFRKj6eUWqi+bGLOX1HL3U3GhC/j0Qg90u3sG/1CUt
wC5vOYvfDmFI6oSFXi5ELabWJmT2dKHzBJKa3k9ok+8t9ucRqMd6DZHJ2YCCLlDRKSKv6kDqnw4U
wPdpOMXziC/AMj3Z/lUVX1G7WSHCAWKf1zNS1eLvqr+boEjXuBOitnZ/bDzPHrTOZz0Dew0uowxf
/+sG+NCK3eQJVxqcaJ/vEHKIVd2M+5qL71yJQ+87X6oV3eaYvt3zWZYD6z5vYTcrtij2VZ9Zmni/
UAaHqn9JdsBWLUEpVviYnhimNVvYFZeCXg/IdTQ+x4IRdiXNv5hEewIDAQABAoIBAQDl8Axy9XfW
BLmkzkEiqoSwF0PsmVrPzH9KsnwLGH+QZlvjWd8SWYGN7u1507HvhF5N3drJoVU3O14nDY4TFQAa
LlJ9VM35AApXaLyY1ERrN7u9ALKd2LUwYhM7Km539O4yUFYikE2nIPscEsA5ltpxOgUGCY7b7ez5
NtD6nL1ZKauw7aNXmVAvmJTcuPxWmoktF3gDJKK2wxZuNGcJE0uFQEG4Z3BrWP7yoNuSK3dii2jm
lpPHr0O/KnPQtzI3eguhe0TwUem/eYSdyzMyVx/YpwkzwtYL3sR5k0o9rKQLtvLzfAqdBxBurciz
aaA/L0HIgAmOit1GJA2saMxTVPNhAoGBAPfgv1oeZxgxmotiCcMXFEQEWflzhWYTsXrhUIuz5jFu
a39GLS99ZEErhLdrwj8rDDViRVJ5skOp9zFvlYAHs0xh92ji1E7V/ysnKBfsMrPkk5KSKPrnjndM
oPdevWnVkgJ5jxFuNgxkOLMuG9i53B4yMvDTCRiIPMQ++N2iLDaRAoGBAO9v//mU8eVkQaoANf0Z
oMjW8CN4xwWA2cSEIHkd9AfFkftuv8oyLDCG3ZAf0vrhrrtkrfa7ef+AUb69DNggq4mHQAYBp7L+
k5DKzJrKuO0r+R0YbY9pZD1+/g9dVt91d6LQNepUE/yY2PP5CNoFmjedpLHMOPFdVgqDzDFxU8hL
AoGBANDrr7xAJbqBjHVwIzQ4To9pb4BNeqDndk5Qe7fT3+/H1njGaC0/rXE0Qb7q5ySgnsCb3DvA
cJyRM9SJ7OKlGt0FMSdJD5KG0XPIpAVNwgpXXH5MDJg09KHeh0kXo+QA6viFBi21y340NonnEfdf
54PX4ZGS/Xac1UK+pLkBB+zRAoGAf0AY3H3qKS2lMEI4bzEFoHeK3G895pDaK3TFBVmD7fV0Zhov
17fegFPMwOII8MisYm9ZfT2Z0s5Ro3s5rkt+nvLAdfC/PYPKzTLalpGSwomSNYJcB9HNMlmhkGzc
1JnLYT4iyUyx6pcZBmCd8bD0iwY/FzcgNDaUmbX9+XDvRA0CgYEAkE7pIPlE71qvfJQgoA9em0gI
LAuE4Pu13aKiJnfft7hIjbK+5kyb3TysZvoyDnb3HOKvInK7vXbKuU4ISgxB2bB3HcYzQMGsz1qJ
2gG0N5hvJpzwwhbhXqFKA4zaaSrw622wDniAK5MlIE0tIAKKP4yxNGjoD2QYjhBGuhvkWKaXTyY=
-----END RSA PRIVATE KEY-----"));

            return (AsymmetricCipherKeyPair)reader.ReadObject();
        }

        protected override string Protocol { get { return m_protocol; } }

        protected override string Version { get { return m_version; } }

        private readonly string m_protocol, m_version;

        public RaopResponse(HttpRequest request)
        {
            if (rsaKey == null)
                rsaKey = CreateCryptoKeys();

            Code = 200;
            Reason = "OK";
            m_protocol = request.Protocol;
            m_version = request.Version;
            Headers["Content-Type"] = "application/sdp";

            Headers["CSeq"] = request.Headers["CSeq"];
            Headers["Audio-Jack-Status"] = "connected; type=analog";

            string challenge = request.Headers["Apple-Challenge"];
            if (!string.IsNullOrEmpty(challenge))
            {
                challenge = challenge.Replace(' ', '+');

                using (var ms = new MemoryStream(32))
                using (var w = new BinaryWriter(ms))
                {
                    w.Write(challenge.FromBase64());

                    w.Write(request.Client.Address.GetAddressBytes());

                    w.Write((byte)0x00);
                    w.Write((byte)0xDE);
                    w.Write((byte)0xAD);
                    w.Write((byte)0xBE);
                    w.Write((byte)0xEF);
                    w.Write((byte)0x10);

                    while (ms.Length < 0x20)
                        w.Write((byte)0);

                    var rsa = new Pkcs1Encoding(new RsaEngine());
                    rsa.Init(true, rsaKey.Private);
                    
                    var encrypted = rsa.ProcessBlock(ms.GetBuffer(), 0, (int)ms.Length);

                    Headers["Apple-Response"] = encrypted.ToBase64();
                }
            }
        }

        protected override byte[] GetData()
        {
            return null;
        }
    }
}
