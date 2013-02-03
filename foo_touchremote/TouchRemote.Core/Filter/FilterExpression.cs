using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Linq.Expressions;
using System.Globalization;
using System.Linq.Dynamic;

namespace TouchRemote.Core.Filter
{
    public class FilterExpression<T>
    {
        private static Regex replaceRegex = new Regex(@"(?'op1'[\s,\(]*)\'(?'prop'[a-zA-Z\.-]+)(?<not>!?):(?'value'(?:[^\']|(?<=\\)\')*)\'(?'op2'[\s,\)]*)");
        private static CompareInfo compareInfo = CultureInfo.InvariantCulture.CompareInfo;
        private const CompareOptions compareOpts = CompareOptions.IgnoreCase | CompareOptions.IgnoreKanaType | CompareOptions.IgnoreNonSpace | CompareOptions.IgnoreWidth;
        private readonly Expression<Func<T, bool>> predicate;

        public FilterExpression(string filter)
        {
            if (string.IsNullOrEmpty(filter))
            {
                predicate = null;
                return;
            }

            var output = replaceRegex.Replace(filter, (MatchEvaluator)delegate(Match m)
            {
                StringBuilder b = new StringBuilder(m.Length);

                var op1 = m.Groups["op1"].Value.Replace(" ", " && ").Replace(",", " || ");
                var op2 = m.Groups["op2"].Value.Replace(" ", " && ").Replace(",", " || ");

                var prop = m.Groups["prop"].Value;

                var replacement = PropertyMap.GetReplacement(prop);
                if (replacement == null)
                    throw new NotSupportedException("Property '" + prop + "' is unknown");

                prop = replacement.Name;
                
                var not = m.Groups["not"].Value == "!";
                
                var value = m.Groups["value"].Value;

                value = value.Replace("\\'", "'");
                if (replacement.NeedQuotes)
                    value = value.Replace("\"", "\\\"");

                b.Append(op1);

                if (string.IsNullOrEmpty(value))
                {
                    if (not) b.Append('!');
                    b.Append("String.IsNullOrEmpty(").Append(prop).Append(")");
                }
                else if (!replacement.NeedQuotes)
                {
                    b.Append(prop);
                    b.Append((not) ? " != " : " == ");
                    b.Append(value);
                }
                else if (value.StartsWith("*"))
                {
                    if (value.EndsWith("*"))
                    {
                        value = value.Substring(1, value.Length - 2);

                        b.Append("@0.IndexOf(").Append(prop).Append(", \"").Append(value).Append("\", @1)");
                        b.Append(not ? " < " : " >= ");
                        b.Append('0');
                    }
                    else
                    {
                        value = value.Substring(1);

                        if (not) b.Append('!');
                        b.Append("@0.IsSuffix(").Append(prop).Append(", \"").Append(value).Append("\", @1)");
                    }
                }
                else if (value.EndsWith("*"))
                {
                    value = value.Substring(0, value.Length - 1);

                    if (not) b.Append('!');
                    b.Append("@0.IsPrefix(").Append(prop).Append(", \"").Append(value).Append("\", @1)");
                }
                else
                {
                    b.Append(prop);
                    b.Append((not) ? " != " : " == ");
                    b.Append("\"").Append(value).Append("\"");
                }

                b.Append(op2);

                return b.ToString();
            });

            FilterString = output;

            predicate = System.Linq.Dynamic.DynamicExpression.ParseLambda<T, bool>(output, compareInfo, compareOpts);
        }

        public string FilterString { get; private set; }

        public IEnumerable<T> Filter(IEnumerable<T> source)
        {
            if (source == null || predicate == null) return source;

            return source.AsQueryable().Where(predicate);
        }

        public IQueryable<T> Filter(IQueryable<T> source)
        {
            if (source == null || predicate == null) return source;

            return source.Where(predicate);
        }

        public bool IsMatch(T item)
        {
            if (item == null) return false;
            if (predicate == null) return true;

            return predicate.Compile()(item);
        }

    }
}
