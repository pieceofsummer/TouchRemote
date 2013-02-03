using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace TouchRemote.Core.Misc
{

    /// <summary>
    /// RWL wrapper for implementations of <see cref="TouchRemote.Interfaces.IReadWriteObject"/> to return
    /// </summary>
    public class ReadWriteLock : IDisposable
    {
        private readonly ReaderWriterLockSlim lockObject;
        private readonly bool forWrite;
        
        public ReadWriteLock(ReaderWriterLockSlim lockObject, bool forWrite)
        {
            if (lockObject == null)
                throw new ArgumentNullException("lockObject");
            this.lockObject = lockObject;
            this.forWrite = forWrite;

            if (forWrite)
                lockObject.EnterWriteLock();
            else
                lockObject.EnterReadLock();
        }

        public void Dispose()
        {
            if (forWrite)
                lockObject.ExitWriteLock();
            else
                lockObject.ExitReadLock();
        }

    }
}
