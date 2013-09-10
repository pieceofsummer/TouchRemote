using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;
using System.Net;
using System.Linq;
using System.Collections;
using System.Collections.ObjectModel;
using System.IO.Compression;

namespace TouchRemote.Core.Dacp
{
    internal static class DataSerializer
    {

        static readonly Dictionary<Type, int> m_estimates = new Dictionary<Type, int>();

        public static byte[] Serialize(object value, bool withCompression)
        {
            if (value == null)
                throw new ArgumentNullException("value");

            int initialSize;
            if (!m_estimates.TryGetValue(value.GetType(), out initialSize))
                initialSize = 128;

            using (var ms = new MemoryStream(initialSize))
            {
                if (withCompression)
                {
                    using (var gzip = new GZipStream(ms, CompressionMode.Compress, true))
                        Serialize(value, gzip);
                }
                else
                    Serialize(value, ms);

                m_estimates[value.GetType()] = Math.Max(initialSize, (int)ms.Length);

                return ms.ToArray();
            }
        }

        public static byte[] Serialize(object value)
        {
            return Serialize(value, false);
        }

        private static void Serialize(object value, Stream stream)
        {
            if (value == null)
                throw new ArgumentNullException("value");
            if (stream == null)
                throw new ArgumentNullException("stream");
            if (!stream.CanWrite/* || !stream.CanSeek*/)
                throw new ArgumentException("Stream is not suited for serialization", "stream");

            var w = new BinaryWriter(stream, Encoding.UTF8);
            SerializeRoot(w, value);
            w.Flush(); // just flush, do not close to keep underlying stream open
        }

        private enum TokenType
        {
            Raw,
            Boolean,
            SByte,
            Byte,
            Char,
            Short,
            UShort,
            Int,
            UInt,
            Long,
            ULong,
            String,
            List,
            Dictionary
        }

        private static string PropName(PropertyInfo item)
        {
            var attr = (TagNameAttribute)Attribute.GetCustomAttribute(item, typeof(TagNameAttribute));
            if (attr != null)
                return attr.Name;

            return item.Name;
        }

        private class MembersCollection : KeyedCollection<string, PropertyInfo>
        {
            protected override string GetKeyForItem(PropertyInfo item)
            {
                return PropName(item);
            }
        }

        private class TypeMetadata
        {
            public TokenType Type { get; set; }
            public MembersCollection Members { get; set; } // this one is valid only for Dictionary tokens
        }

        private static readonly Dictionary<Type, TypeMetadata> typeMetadataCache = new Dictionary<Type, TypeMetadata>();
        private static readonly Dictionary<Type, object> typeMetadataLocks = new Dictionary<Type, object>();

        private static TypeMetadata ResolveType(Type type)
        {
            TypeMetadata metadata;
            object metadataLock;

            lock (typeMetadataCache)
            {
                if (typeMetadataCache.TryGetValue(type, out metadata)) return metadata;

                lock (typeMetadataLocks)
                    if (!typeMetadataLocks.TryGetValue(type, out metadataLock))
                    {
                        metadataLock = new object();
                        typeMetadataLocks.Add(type, metadataLock);
                    }
            }

            lock (metadataLock)
            {
                metadata = ResolveTypeNolock(type);
                
                lock (typeMetadataCache)
                    typeMetadataCache[type] = metadata;

                lock (typeMetadataLocks)
                    typeMetadataLocks.Remove(type);
            }

            return metadata;
        }

        private static TypeMetadata ResolveTypeNolock(Type type)
        {
            var metadata = new TypeMetadata();

            if (type == typeof(bool))
            {
                metadata.Type = TokenType.Boolean;
            }
            else if (type == typeof(sbyte))
            {
                metadata.Type = TokenType.SByte;
            }
            else if (type == typeof(byte))
            {
                metadata.Type = TokenType.Byte;
            }
            else if (type == typeof(char))
            {
                metadata.Type = TokenType.Char;
            }
            else if (type == typeof(short))
            {
                metadata.Type = TokenType.Short;
            }
            else if (type == typeof(ushort))
            {
                metadata.Type = TokenType.UShort;
            }
            else if (type == typeof(int))
            {
                metadata.Type = TokenType.Int;
            }
            else if (type == typeof(uint))
            {
                metadata.Type = TokenType.UInt;
            }
            else if (type == typeof(long))
            {
                metadata.Type = TokenType.Long;
            }
            else if (type == typeof(ulong))
            {
                metadata.Type = TokenType.ULong;
            }
            else if (type == typeof(string))
            {
                metadata.Type = TokenType.String;
            }
            else if (type == typeof(byte[]))
            {
                metadata.Type = TokenType.Raw;
            }
            else if (typeof(IDictionary).IsAssignableFrom(type))
            {
                metadata.Type = TokenType.Dictionary;
            }
            else if (typeof(ICollection).IsAssignableFrom(type))
            {
                metadata.Type = TokenType.List;
            }
            else
            {
                metadata.Type = TokenType.Dictionary;
                metadata.Members = new MembersCollection();

                var props = type.GetProperties(BindingFlags.Instance | BindingFlags.Public | BindingFlags.DeclaredOnly);
                foreach (var prop in props)
                    metadata.Members.Add(prop);
            }

            return metadata;
        }

        private static void SerializeRoot(BinaryWriter stream, object value)
        {
            if (value == null)
                throw new ArgumentNullException("value");

            var metadata = ResolveType(value.GetType());
            if (metadata == null)
                throw new ApplicationException("Everything's screwed up!");

            if (metadata.Type != TokenType.Dictionary)
                throw new ArgumentException("Not a Dictionary structure", "value");

            if (metadata.Members.Count != 1)
                throw new ArgumentNullException("Root element must have exactly one member", "value");

            var item = metadata.Members.First();
            var itemValue = item.GetValue(value, null);

            SerializeValue(stream, item.Name, itemValue, metadata);
        }

        private static void SerializeDictionary(BinaryWriter stream, object value, TypeMetadata metadata)
        {
            if (metadata.Members != null)
            {
                // this is a reflected dictionary

                foreach (var item in metadata.Members)
                {
                    var itemName = PropName(item);
                    var itemValue = item.GetValue(value, null);
                    SerializeValue(stream, itemName, itemValue, metadata);
                }
            }
            else
            {
                // this is a true dictionary

                foreach (DictionaryEntry pair in (IDictionary)value)
                {
                    SerializeValue(stream, pair.Key as string, pair.Value, metadata);
                }
            }
        }

        private static void SerializeList(BinaryWriter stream, ICollection value, TypeMetadata metadata, string listName)
        {
            var extender = value as IListExtender;
            if (extender != null)
            {
                var extension = extender.Extend();
                if (extension != null)
                {
                    var extMetadata = ResolveType(extension.GetType());
                    if (metadata == null)
                        throw new ApplicationException("Everything's screwed up!");

                    SerializeDictionary(stream, extension, extMetadata);
                }
            }

            foreach (var item in value)
            {
                if (item == null) continue;
                SerializeValue(stream, "mlit", item, metadata);
            }
        }

        private static void SerializeValue(BinaryWriter stream, string name, object value, TypeMetadata parent)
        {
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length != 4)
                throw new ArgumentException("Name length mismatch (" + name + ")", "name");

            if (value == null)
                return;

            var metadata = ResolveType(value.GetType());
            if (metadata == null)
                throw new ApplicationException("Everything's screwed up!");

            if (metadata.Type != TokenType.List)
            {
                // list tag would be written later (in case mtco/mrco are added)
                stream.Write(Encoding.ASCII.GetBytes(name));
            }

            switch (metadata.Type)
            {
                case TokenType.Raw:
                    var bytes = (byte[])value;
                    var count = (bytes != null) ? bytes.Length : 0;
                    stream.Write(IPAddress.HostToNetworkOrder(count));
                    stream.Write(bytes);
                    break;
                case TokenType.Boolean:
                    stream.Write(0x01000000);
                    stream.Write((bool)value);
                    break;
                case TokenType.SByte:
                    stream.Write(0x01000000);
                    stream.Write((sbyte)value);
                    break;
                case TokenType.Byte:
                    stream.Write(0x01000000);
                    stream.Write((byte)value);
                    break;
                case TokenType.Char:
                    stream.Write(0x02000000);
                    stream.Write(IPAddress.HostToNetworkOrder(unchecked((short)(char)value)));
                    break;
                case TokenType.Short:
                    stream.Write(0x02000000);
                    stream.Write(IPAddress.HostToNetworkOrder((short)value));
                    break;
                case TokenType.UShort:
                    stream.Write(0x02000000);
                    stream.Write(IPAddress.HostToNetworkOrder(unchecked((short)(ushort)value)));
                    break;
                case TokenType.Int:
                    stream.Write(0x04000000);
                    stream.Write(IPAddress.HostToNetworkOrder((int)value));
                    break;
                case TokenType.UInt:
                    stream.Write(0x04000000);
                    stream.Write(IPAddress.HostToNetworkOrder(unchecked((int)(uint)value)));
                    break;
                case TokenType.Long:
                    stream.Write(0x08000000);
                    stream.Write(IPAddress.HostToNetworkOrder((long)value));
                    break;
                case TokenType.ULong:
                    stream.Write(0x08000000);
                    stream.Write(IPAddress.HostToNetworkOrder(unchecked((long)(ulong)value)));
                    break;
                case TokenType.String:
                    var str = (string)value;
                    if (!string.IsNullOrEmpty(str))
                    {
                        var strBytes = Encoding.UTF8.GetBytes(str);
                        stream.Write(IPAddress.HostToNetworkOrder(strBytes.Length));
                        stream.Write(strBytes);
                    }
                    else
                        stream.Write(0);
                    break;
                case TokenType.List:
                    var list = (ICollection)value;
                    
                    if (parent != null && parent.Type == TokenType.Dictionary)
                    {
                        if (!parent.Members.Contains("mtco"))
                            SerializeValue(stream, "mtco", list.Count, metadata);
                        if (!parent.Members.Contains("mrco"))
                            SerializeValue(stream, "mrco", list.Count, metadata);
                    }

                    stream.Write(Encoding.ASCII.GetBytes(name));
                    stream.Write(IPAddress.HostToNetworkOrder(MeasureList(list, metadata)));
                    SerializeList(stream, list, metadata, name);
                    break;
                case TokenType.Dictionary:
                    stream.Write(IPAddress.HostToNetworkOrder(MeasureDictionary(value, metadata)));
                    SerializeDictionary(stream, value, metadata);
                    break;
            }
        }

        private static int MeasureList(ICollection value, TypeMetadata metadata)
        {
            var size = 0;

            var extender = value as IListExtender;
            if (extender != null)
            {
                var extension = extender.Extend();
                if (extension != null)
                {
                    var extMetadata = ResolveType(extension.GetType());
                    if (metadata == null)
                        throw new ApplicationException("Everything's screwed up!");

                    size += MeasureDictionary(extension, extMetadata);
                }
            }

            foreach (var item in value)
            {
                if (item == null) continue;
                size += MeasureValue("mlit", item, metadata);
            }

            return size;
        }

        private static int MeasureDictionary(object value, TypeMetadata metadata)
        {
            var size = 0;

            if (metadata.Members != null)
            {
                // this is a reflected dictionary

                foreach (var item in metadata.Members)
                {
                    var itemName = PropName(item);
                    var itemValue = item.GetValue(value, null);
                    size += MeasureValue(itemName, itemValue, metadata);
                }
            }
            else
            {
                // this is a true dictionary

                foreach (DictionaryEntry pair in (IDictionary)value)
                {
                    size += MeasureValue(pair.Key as string, pair.Value, metadata);
                }
            }

            return size;
        }

        private static int MeasureValue(string name, object value, TypeMetadata parent)
        {
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length != 4)
                throw new ArgumentException("Name length mismatch (" + name + ")", "name");

            if (value == null)
                return 0;

            var metadata = ResolveType(value.GetType());
            if (metadata == null)
                throw new ApplicationException("Everything's screwed up!");

            var size = 8;

            switch (metadata.Type)
            {
                case TokenType.Raw:
                    var bytes = (byte[])value;
                    size += ((bytes != null) ? bytes.Length : 0);
                    break;
                case TokenType.Boolean:
                case TokenType.SByte:
                case TokenType.Byte:
                    size += 1;
                    break;
                case TokenType.Char:
                case TokenType.Short:
                case TokenType.UShort:
                    size += 2;
                    break;
                case TokenType.Int:
                case TokenType.UInt:
                    size += 4;
                    break;
                case TokenType.Long:
                case TokenType.ULong:
                    size += 8;
                    break;
                case TokenType.String:
                    var str = (string)value;
                    size += ((!string.IsNullOrEmpty(str)) ? Encoding.UTF8.GetByteCount(str) : 0);
                    break;
                case TokenType.List:
                    var list = (ICollection)value;

                    if (parent != null && parent.Type == TokenType.Dictionary)
                    {
                        if (!parent.Members.Contains("mtco"))
                            size += 12;
                        if (!parent.Members.Contains("mrco"))
                            size += 12;
                    }

                    size += MeasureList(list, metadata);
                    break;
                case TokenType.Dictionary:
                    size += MeasureDictionary(value, metadata);
                    break;
            }

            return size;
        }

    }
}
