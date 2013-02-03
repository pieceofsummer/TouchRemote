using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Interfaces
{
    public interface IPropertyExtender
    {
        void Extend(IDictionary<string, object> data);
    }
}
