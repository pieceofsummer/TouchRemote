using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace TouchRemote.Core
{
    internal class Session
    {

        public Session(int sessionId, bool guestMode)
        {
            SessionId = sessionId;
            GuestMode = guestMode;
            m_dbLocks = 0;
            m_stateLocks = 0;
            CtrlIntRevision = 0;
            DatabaseRevision = 0;
        }

        public int SessionId { get; private set; }

        public bool GuestMode { get; private set; }

        public uint CtrlIntRevision { get; set; }
        public uint DatabaseRevision { get; set; }

        #region Session locking

        private class SessionLock : IDisposable
        {
            private readonly Session session;
            private readonly int mode;

            public SessionLock(Session session, int mode)
            {
                this.session = session;
                this.mode = mode;
                if ((mode & 1) == 1)
                    Interlocked.Increment(ref session.m_stateLocks);
                if ((mode & 2) == 2)
                    Interlocked.Increment(ref session.m_dbLocks);
            }

            public void Dispose()
            {
                if ((mode & 1) == 1)
                    Interlocked.Decrement(ref session.m_stateLocks);
                if ((mode & 2) == 2)
                    Interlocked.Decrement(ref session.m_dbLocks);
            }
        }

        private volatile int m_stateLocks;
        private volatile int m_dbLocks;

        public bool IsStateLocked { get { return m_stateLocks > 0; } }

        public bool IsDbLocked { get { return m_dbLocks > 0; } }

        public IDisposable LockState()
        {
            return new SessionLock(this, 1);
        }

        public IDisposable LockDatabase()
        {
            return new SessionLock(this, 2);
        }

        #endregion
    }
}
