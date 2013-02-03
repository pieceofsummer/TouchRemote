using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Specialized;
using System.IO;

namespace TouchRemote.Bonjour
{
    internal static class TxtUtils
    {

        public static byte[] ToArray(this NameValueCollection data)
        {
            if (data == null)
                throw new ArgumentNullException("data");

            using (var ms = new MemoryStream())
            {
                foreach (string key in data)
                {
                    var value = data[key];

                    string rawValue;
                    if (value == null)
                        rawValue = key;
                    else
                        rawValue = key + "=" + value;

                    var rawData = Encoding.UTF8.GetBytes(rawValue);
                    if (rawData.Length > 255)
                        throw new ArgumentException("Value '" + rawValue + "' is too long", "data");

                    ms.WriteByte((byte)rawData.Length);
                    ms.Write(rawData, 0, rawData.Length);
                }

                return ms.ToArray();
            }
        }

        public static NameValueCollection FromArray(this byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException("data");

            var dict = new NameValueCollection(StringComparer.Ordinal);

            int byteRead = 0;
            while (byteRead < data.Length)
            {
                byte propertyLength = data[byteRead];

                var rawValue = Encoding.UTF8.GetString(data, byteRead + 1, propertyLength);

                var idx = rawValue.IndexOf('=');
                if (idx == -1)
                    dict.Add(rawValue, null);
                else
                    dict.Add(rawValue.Substring(0, idx), rawValue.Substring(idx + 1));

                byteRead += (propertyLength + 1);
            }

            return dict;
        }

    }
}
