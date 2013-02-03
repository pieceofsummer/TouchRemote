using System;
using System.Collections.Generic;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IAlbum : IReferenceItem, IComparable<IAlbum>, IEquatable<IAlbum>
    {
        
        IArtist Artist { get; }

        string Title { get; }

    }
}
