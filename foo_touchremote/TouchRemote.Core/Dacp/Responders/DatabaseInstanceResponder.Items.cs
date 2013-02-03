using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using TouchRemote.Interfaces;
using TouchRemote.Core.Http;
using TouchRemote.Core.Filter;
using TouchRemote.Core.Misc;

namespace TouchRemote.Core.Dacp.Responders
{
    internal partial class DatabaseInstanceResponder
    {

        private HttpResponse GetItemsResponse()
        {
            if (!id2.HasValue)
                return LibraryItems();

            switch (query2)
            {
                case "extra_data/artwork":
                    return ItemArtwork();

                default:
                    return null;
            }
        }

        private HttpResponse ItemArtwork()
        {
            using (Player.MediaLibrary.BeginRead())
            {
                IArtworkSource[] items = Player.MediaLibrary.Tracks.Where(x => x.Id == id2.Value).OfType<IArtworkSource>().ToArray();
                return Request.GetArtwork(items);
            }
        }

    }
}
