namespace MotionDetector
{
    using System;
    using System.IO;

    public static class StringExtensions
    {
        static readonly string[] AvailableExtensions = {
            ".ts", ".avi", ".mp4"
        };

        public static bool IsVideoFile(this string file)
        {
            var fileExtension = Path.GetExtension(file);
            foreach (var extension in AvailableExtensions)
            {
                if (fileExtension.Equals(extension, StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }

            return false;
        }

        public static bool IsUrl(this string path)
        {
            Uri uriResult;
            return Uri.TryCreate(path, UriKind.Absolute, out uriResult)
                && (uriResult.Scheme == Uri.UriSchemeHttp ||
                uriResult.Scheme == Uri.UriSchemeHttps);
        }
    }
}
