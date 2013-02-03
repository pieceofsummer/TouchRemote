using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;

namespace TouchRemote.Core.Library
{
    public abstract class SpecialPlaylistBase : IPlaylist, IPropertyExtender
    {
        private readonly int m_id;
        protected readonly PlaylistType m_type;
        private readonly string m_name;
        protected readonly IMediaLibrary m_library;

        protected SpecialPlaylistBase(IMediaLibrary library, PlaylistType type, string name)
        {
            if (library == null)
                throw new ArgumentNullException("library");
            if (type == PlaylistType.None)
                throw new ArgumentException("type");
            if (string.IsNullOrEmpty(name))
                name = type.ToString();

            m_library = library;
            m_type = type;
            m_id = library.Id + (byte)type;
            m_name = name;
        }
        
        public int Id
        {
            get { return m_id; }
        }

        public long PersistentId
        {
            get { return m_id; }
        }

        public string Name
        {
            get
            {
                return m_name;
            }
            set
            {
                throw new InvalidOperationException("Cannot rename special playlist");
            }
        }

        public PlaylistType Type
        {
            get { return m_type; }
        }

        public virtual int TrackCount
        {
            get { return (Tracks != null) ? Tracks.Count() : 0; }
        }

        public abstract IEnumerable<ITrack> Tracks { get; }

        public bool Equals(IPlaylist other)
        {
            if (ReferenceEquals(other, null)) return false;
            if (ReferenceEquals(other, this)) return true;

            var sp = other as SpecialPlaylistBase;
            if (sp == null) return false;

            return sp.m_library == m_library && sp.m_type == m_type;
        }

        public override int GetHashCode()
        {
            return (m_library.GetHashCode() << 8) ^ (byte)m_type;
        }

        public override bool Equals(object obj)
        {
            return Equals(obj as IPlaylist);
        }

        public virtual void Extend(IDictionary<string, object> data)
        {
        }
    }
}
