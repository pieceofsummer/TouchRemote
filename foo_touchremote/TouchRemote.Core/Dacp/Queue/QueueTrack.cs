using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Dacp.Queue
{
    internal class QueueTrack
    {
        [TagName("ceQs")]
        public byte[] TrackSpecs { get; set; }

        [TagName("ceQn")]
        public string Title { get; set; }

        [TagName("ceQr")]
        public string Artist { get; set; }

        [TagName("ceQa")]
        public string Album { get; set; }

        [TagName("ceQg")]
        public string Genre { get; set; }

        [TagName("asai")]
        public long AlbumId { get; set; }

        // not sure if int
        [TagName("casa")]
        public int AlbumArtist { get; set; }

        [TagName("cmmk")]
        public int MediaKind { get; set; }

        [TagName("ceQI")]
        public int QueueIndex { get; set; }

        // not sure
        [TagName("ceMQ")]
        public bool Upcoming { get; set; }

    }
}
