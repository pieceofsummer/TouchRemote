using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Core.Dacp.Responders;

namespace TouchRemote.Core
{
    public static class SessionManager
    {

        private static readonly Dictionary<int, Session> sessions = new Dictionary<int, Session>();

        internal static Session GetSession(int sessionId)
        {
            lock (sessions)
            {
                Session session;
                if (sessions.TryGetValue(sessionId, out session))
                    return session;
            }

            return null;
        }

        internal static void StartSession(int sessionId, bool guestMode)
        {
            lock (sessions)
            {
                Session info;
                if (sessions.TryGetValue(sessionId, out info))
                {
                    // TODO: do something with it
                }
                else
                {
                    info = new Session(sessionId, guestMode);
                    sessions.Add(sessionId, info);
                }
            }
        }

        internal static void TerminateSession(int sessionId)
        {
            lock (sessions)
            {
                sessions.Remove(sessionId);
            }
        }

        internal static void TerminateSession(Session session)
        {
            if (session != null)
                TerminateSession(session.SessionId);
        }

        public static void DatabaseUpdated()
        {
            lock (sessions)
            {
                foreach (var session in sessions.Where(x => !x.Value.IsDbLocked))
                    session.Value.DatabaseRevision++;
            }
        }

        public static void StateUpdated()
        {
            lock (sessions)
            {
                foreach (var session in sessions.Where(x => !x.Value.IsStateLocked))
                    session.Value.CtrlIntRevision++;
            }
        }
        
    }
}
