using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Http;
using System.Text.RegularExpressions;
using TouchRemote.Core.Http.Response;
using System.Linq;
using TouchRemote.Interfaces;
using System.Collections;
using TouchRemote.Core.Filter;
using System.IO;
using System.Drawing.Imaging;
using System.Drawing;
using System.Drawing.Drawing2D;
using TouchRemote.Core.Library;
using TouchRemote.Core.Misc;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder : SessionBoundResponder
    {
        private static readonly Regex databaseRegex = new Regex(@"^/databases/(\d+)/([a-z]+)(/(\d+)/((extra_data/)?[a-z]+))?$");
        private static readonly Regex databaseRegex2 = new Regex(@"^/databases/(\d+)/browse/([a-z]+)$");

        private int id;
        private string query;
        private int? id2;
        private string query2;

        public DatabaseInstanceResponder(HttpRequest request) : base(request)
        {
            var match = databaseRegex.Match(request.Path);
            if (match.Success)
            {
                id = int.Parse(match.Groups[1].Value);
                query = match.Groups[2].Value;
                if (match.Groups[3].Success)
                {
                    id2 = int.Parse(match.Groups[4].Value);
                    query2 = match.Groups[5].Value;
                }
            }
            else
            {
                match = databaseRegex2.Match(request.Path);
                if (match.Success)
                {
                    id = int.Parse(match.Groups[1].Value);
                    query = "browse";
                    query2 = match.Groups[2].Value;
                }
                else
                    throw new ArgumentException("Not a valid request for this responder", "request");
            }
        }

        public override HttpResponse GetResponse()
        {
            switch (query)
            {
                case "containers":
                    return GetContainersResponse();

                case "groups":
                    return GetGroupsResponse();

                case "browse":
                    return GetBrowseResponse();

                case "items":
                    return GetItemsResponse();

                case "edit":
                    return EditDatabaseResponse();

                default:
                    return null;
            }
        }

    }
}
