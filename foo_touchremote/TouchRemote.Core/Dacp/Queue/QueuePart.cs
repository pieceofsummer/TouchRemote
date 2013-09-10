using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Dacp.Queue
{
    internal class QueuePart
    {
        // possible values: 
        // 'hist' = previously played, 
        // 'curr' = current song, 
        // 'main' = UpNext queue, 
        // 'subm' = back to
        [TagName("ceQk")]
        public string PartType { get; set; }

        // probably item index
        [TagName("ceQi")]
        public int Index { get; set; }

        // probably items after
        [TagName("ceQm")]
        public int PartCount { get; set; }

        // optional label ('curr' doesn't have one)
        [TagName("ceQl")]
        public string Label { get; set; }

        // optional hint
        [TagName("ceQh")]
        public string Hint { get; set; }
    }
}
