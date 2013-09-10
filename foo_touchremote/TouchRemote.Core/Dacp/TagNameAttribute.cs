using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Dacp
{

    [AttributeUsage(AttributeTargets.Property | AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    public class TagNameAttribute : Attribute
    {
        private readonly string name;

        public TagNameAttribute(string name)
        {
            this.name = name;
        }

        public string Name { get { return name; } }

    }
}
