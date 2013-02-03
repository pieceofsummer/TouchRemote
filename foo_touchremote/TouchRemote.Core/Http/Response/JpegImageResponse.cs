using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace TouchRemote.Core.Http.Response
{
    public class JpegImageResponse : HttpResponse
    {
        private byte[] data;

        public JpegImageResponse(byte[] data)
        {
            if (data == null)
                throw new ArgumentNullException("data");
            if (data.Length == 0)
                throw new ArgumentException("Data array is empty", "data");

            this.data = data;
            PostInit();
        }

        private void PostInit()
        {
            Code = 200;
            Reason = "OK";
            Headers["Content-Type"] = "image/jpeg";
            Headers["Cache-Control"] = "no-cache";
        }

        protected override byte[] GetData()
        {
            return data;
        }
    }
}
