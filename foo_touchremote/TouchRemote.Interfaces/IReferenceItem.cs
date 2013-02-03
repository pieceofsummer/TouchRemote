using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IReferenceItem
    {

        int Id { get; }

        long PersistentId { get; }

    }
}
