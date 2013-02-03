using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core
{
    internal static class Base64Extensions
    {

        public static byte[] FromBase64(this string s)
        {
            if (string.IsNullOrEmpty(s))
                throw new ArgumentNullException("s");

            if ((s.Length % 4) != 0)
                s += new string('=', 4 - (s.Length % 4));

            return Convert.FromBase64String(s);
        }

        public static string ToBase64(this byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException("data");

            var s = Convert.ToBase64String(data);

            return s.TrimEnd('=');
        }

    }
}
