using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace TouchRemote.Core.Dacp.Queue
{
    internal class QueueTracks : ArrayList, IListExtender
    {
        public QueueTracks()
        {
            
        }

        public object Extend()
        {
            return new
            {
                ceQS = new QueuePart[] {
                    new QueuePart() { PartType = "hist", Index = -1, PartCount = 0, Label = "Previously played" },
                    new QueuePart() { PartType = "curr", Index = 0, PartCount = 1 },
                    new QueuePart() { PartType = "main", Index = 1, PartCount = 1, Label = "Up Next: ", Hint = "1 songs" },
                    new QueuePart() { PartType = "subm", Index = 2, PartCount = 0, Label = "Back to: ", Hint = "music" }
                }
            };
        }
    }
}
