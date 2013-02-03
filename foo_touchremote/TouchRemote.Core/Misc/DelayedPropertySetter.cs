using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;

namespace TouchRemote.Core.Misc
{
    public class DelayedPropertySetter<T>
    {
        private readonly Timer m_timer;
        private T m_immediateValue;

        public DelayedPropertySetter()
        {
            m_timer = new Timer(100);
            m_timer.Enabled = false;
            m_timer.AutoReset = false;
            m_timer.Elapsed += OnTimerElapsed;
        }

        private void OnTimerElapsed(object sender, ElapsedEventArgs e)
        {
            lock (m_timer)
            {
                var action = Setter;
                if (action != null)
                    action(m_immediateValue);
            }
        }

        public double Delay 
        {
            get { return m_timer.Interval; }
            set 
            {
                if (m_timer.Interval != value)
                {
                    lock (m_timer)
                    {
                        m_timer.Stop();
                        m_timer.Interval = value;
                    }
                }
            }
        }

        public Action<T> Setter { get; set; }

        public void SetValue(T value)
        {
            lock (m_timer)
            {
                m_timer.Stop();
                m_immediateValue = value;
                m_timer.Start();
            }
        }
    }
}
