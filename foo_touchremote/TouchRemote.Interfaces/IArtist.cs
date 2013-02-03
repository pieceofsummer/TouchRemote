using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IArtist : IReferenceItem, IEquatable<IArtist>, IComparable<IArtist>
    {

        string Name { get; }

    }
}
