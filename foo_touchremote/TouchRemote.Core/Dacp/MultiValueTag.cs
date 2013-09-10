using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace TouchRemote.Core.Dacp
{
    internal class MultiValueTag : IDictionary
    {
        private class MultiValueTagEnumerator : IDictionaryEnumerator
        {
            private int index;
            private readonly MultiValueTag source;

            public MultiValueTagEnumerator(MultiValueTag source)
            {
                this.source = source;
                index = -1;
            }

            private void CheckIndexValid()
            {
                if (index < 0 || index >= source.Count)
                    throw new InvalidOperationException();
            }

            public DictionaryEntry Entry
            {
                get
                {
                    CheckIndexValid();
                    var pair = source.values[index];
                    return new DictionaryEntry(pair.Key, pair.Value);
                }
            }

            public object Key
            {
                get
                {
                    CheckIndexValid();
                    return source.values[index].Key;
                }
            }

            public object Value
            {
                get
                {
                    CheckIndexValid();
                    return source.values[index].Value;
                }
            }

            public object Current { get { return Entry; } }

            public bool MoveNext()
            {
                return (++index < source.Count);
            }

            public void Reset()
            {
                index = -1;
            }

        }

        private readonly List<KeyValuePair<string, object>> values;

        public MultiValueTag()
        {
            this.values = new List<KeyValuePair<string, object>>();
        }

        private void ValidateKey(object key)
        {
            if (key == null)
                throw new ArgumentNullException("key");
            if (key.GetType() != typeof(string) || ((string)key).Length != 4)
                throw new ArgumentException("Invalid key", "key");
        }

        public void Add(object key, object value)
        {
            ValidateKey(key);
            values.Add(new KeyValuePair<string, object>((string)key, value));
        }

        public void Clear()
        {
            values.Clear();
        }

        public bool Contains(object key)
        {
            ValidateKey(key);
            return values.Any(x => string.Equals(x.Key, (string)key));
        }

        public IDictionaryEnumerator GetEnumerator()
        {
            return new MultiValueTagEnumerator(this);
        }

        public bool IsFixedSize { get { return false; } }

        public bool IsReadOnly { get { return false; } }

        public ICollection Keys
        {
            get { return values.Select(x => x.Key).Distinct().ToArray(); }
        }

        public void Remove(object key)
        {
            ValidateKey(key);

            for (int i = values.Count; i >= 0; i--)
            {
                if (string.Equals(values[i].Key, (string)key))
                    values.RemoveAt(i);
            }
        }

        public ICollection Values
        {
            get { return values.Select(x => x.Value).ToArray(); }
        }

        public object this[object key]
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }


        public void CopyTo(Array array, int index)
        {
            throw new NotImplementedException();
        }

        public int Count { get { return values.Count; } }

        public bool IsSynchronized { get { return false; } }

        public object SyncRoot { get { return values; } }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

    }
}
