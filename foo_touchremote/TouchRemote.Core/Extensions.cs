using System;
using System.Collections.Generic;
using System.Text;
using TouchRemote.Core.Dacp;
using TouchRemote.Core.Http;
using TouchRemote.Interfaces;
using System.Drawing;
using System.IO;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using TouchRemote.Core.Http.Response;
using System.Net.Sockets;

namespace TouchRemote.Core
{
    internal static class Extensions
    {

        private static readonly DateTime unixEpoch = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);

        public static uint ToUnixTime(this DateTime value)
        {
            if (value < unixEpoch)
                throw new ArgumentException("Date is way too ancient", "value");

            return (uint)Math.Round(value.Subtract(unixEpoch).TotalSeconds);
        }

        public static bool IsHexDigit(this char c)
        {
            if (char.IsDigit(c)) return true;
            if (c >= 'A' && c <= 'F') return true;
            return (c >= 'a' && c <= 'f');
        }

        public static bool HasIncomingData(this Socket sock, bool withErrors)
        {
            /*var read = new List<Socket>() { sock };
            var err = (withErrors) ? new List<Socket>() { sock } : null;

            Socket.Select(read, null, err, 100);

            return read.Count > 0 || (err != null && err.Count > 0);*/

            return sock.Poll(100, SelectMode.SelectRead);
        }

        public static ShortcutItem[] GetShortcuts<T>(this IEnumerable<T> source, Func<T, string> selector)
        {
            if (source == null) return null;

            char lastLetter = '\0';
            int lastStart = 0;
            int lastCount = 0;
            int counter = 0;
            bool lettersFinished = false;

            var items = new List<ShortcutItem>();

            foreach (var item in source)
            {
                var title = selector(item);
                if (!string.IsNullOrEmpty(title))
                {

                    if (lastLetter != char.ToUpper(title[0]))
                    {
                        if (lastCount > 0)
                        {
                            //if (lastLetter >= 'A' && lastLetter <= 'Z')
                            items.Add(new ShortcutItem { mshc = lastLetter, mshi = lastStart, mshn = lastCount });
                            //else
                            //    items.Add(new ShortcutItem { mshc = '#', mshi = lastStart, mshn = lastCount });
                        }

                        lastStart = counter;
                        lastCount = 0;
                        lastLetter = char.ToUpper(title[0]);

                        //if (lastLetter < 'A' || lastLetter > 'Z')
                        //    lettersFinished = true;
                    }

                    lastCount++;
                }

                counter++;
            }

            if (lastCount > 0)
            {
                //if (lastLetter >= 'A' && lastLetter <= 'Z')
                    items.Add(new ShortcutItem { mshc = lastLetter, mshi = lastStart, mshn = lastCount });
                //else
                //    items.Add(new ShortcutItem { mshc = '#', mshi = lastStart, mshn = lastCount });
            }

            return items.ToArray();
        }

        public static HttpResponse GetArtwork(this HttpRequest request, IEnumerable<IArtworkSource> source)
        {
            if (source == null) return new NoContentResponse();

            foreach (var item in source)
            {
                using (var b = item.GetCoverImage())
                {
                    if (b == null) continue;

                    int maxWidth, maxHeight;

                    if (string.IsNullOrEmpty(request.QueryString["mw"]) || !int.TryParse(request.QueryString["mw"], out maxWidth))
                        maxWidth = 0;
                    if (string.IsNullOrEmpty(request.QueryString["mh"]) || !int.TryParse(request.QueryString["mh"], out maxHeight))
                        maxHeight = 0;

                    Bitmap final = b;

                    int width = b.Width;
                    int height = b.Height;
                    bool resize = false;

                    if (maxWidth > 0 && maxHeight > 0)
                    { 
                        double aspect = (double)width / height;

                        if (width > maxWidth)
                        {
                            width = maxWidth;
                            height = (int)((double)maxWidth / aspect);
                            resize = true;
                        }

                        if (height > maxHeight)
                        {
                            height = maxHeight;
                            width = (int)((double)maxHeight * aspect);
                            resize = true;
                        }
                    }

                    //if (resize)
                    {
                        using (var b2 = new Bitmap(width, height, PixelFormat.Format24bppRgb))
                        {
                            using (var g = Graphics.FromImage(b2))
                            {
                                g.SmoothingMode = SmoothingMode.AntiAlias;
                                g.InterpolationMode = InterpolationMode.Bicubic;

                                g.DrawImage(b, 0, 0, width, height);
                            }

                            using (var ms = new MemoryStream(4096))
                            {
                                b2.Save(ms, ImageFormat.Jpeg);
                                return new JpegImageResponse(ms.ToArray());
                            }
                        }
                    }
                    /*else
                    {
                        using (var ms = new MemoryStream(4096))
                        {
                            b.Save(ms, ImageFormat.Jpeg);
                            return new JpegImageResponse(ms.ToArray());
                        }
                    }*/
                }
            }

            return new NoContentResponse();
        }

        public static int IndexOfFirst<T>(this IEnumerable<T> source, Predicate<T> filter)
        {
            if (source == null) return -1;

            if (filter == null)
                throw new ArgumentNullException("filter");

            int index = -1, found = -1;

            foreach (var item in source)
            {
                index++;
                if (filter(item))
                {
                    found = index;
                    break;
                }
            }

            return found;
        }

        public static IEnumerable<IPlaylist> GetContainers(this IPlayer player)
        {
            yield return player.MediaLibrary;

            if (player.MediaLibrary.Podcasts != null)
                yield return player.MediaLibrary.Podcasts;
            if (player.MediaLibrary.Jukebox != null)
                yield return player.MediaLibrary.Jukebox;
            if (player.MediaLibrary.Music != null)
                yield return player.MediaLibrary.Music;
            if (player.MediaLibrary.Videos != null)
                yield return player.MediaLibrary.Videos;
            if (player.MediaLibrary.AudioBooks != null)
                yield return player.MediaLibrary.AudioBooks;
            foreach (var p in player.Playlists)
                yield return p;
        }

    }
}
