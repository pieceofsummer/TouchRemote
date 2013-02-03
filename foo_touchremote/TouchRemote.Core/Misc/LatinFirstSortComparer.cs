using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TouchRemote.Core.Misc
{
    public class LatinFirstSortComparer : IComparer<string>
    {
        private static readonly LatinFirstSortComparer instance = new LatinFirstSortComparer();

        public static LatinFirstSortComparer Instance
        {
            get { return instance; }
        }

        private readonly IComparer<string> defaultComparer;

        public LatinFirstSortComparer()
        {
            defaultComparer = StringComparer.InvariantCultureIgnoreCase;
        }

        private static bool IsLatinLetter(char letter)
        {
            return (letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z');
        }

        public int Compare(string x, string y)
        {
            if (string.IsNullOrEmpty(x))
            {
                return string.IsNullOrEmpty(y) ? 0 : -1;
            }
            else if (string.IsNullOrEmpty(y))
            {
                return 1;
            }
            else
            {
                if (IsLatinLetter(x[0]))
                {
                    if (IsLatinLetter(y[0]))
                    {
                        return defaultComparer.Compare(x, y);
                    }
                    else
                    {
                        return -1;
                    }
                }
                else if (IsLatinLetter(y[0]))
                {
                    return 1;
                }
                else
                {
                    return defaultComparer.Compare(x, y);
                }
            }
        }

    }
}
