namespace MotionDetector
{
    using System;
    using System.IO;
    using Mono.Options;
    using MotionDetectorLibrary;

    class Program
    {
        static void DetectFileOrUrl(string name, string to, double k)
        {
            var isUrl = name.IsUrl();
            if (!isUrl && !File.Exists(name))
            {
                throw new FileNotFoundException("File not found", name);
            }

            if (string.IsNullOrWhiteSpace(to))
            {
                to = isUrl ? Environment.CurrentDirectory : Path.GetFullPath(name);
            }
            else
            {
                Directory.CreateDirectory(to);
            }

            Console.WriteLine($@"Start work for {(isUrl ? "url" : "file")}: {name}
k = {k}
Files save to: {to}");

            Detector.Detect(name, to, k);
        }

        static void DetectDirectory(string name, string to, double k)
        {
            Console.WriteLine($"Start work in directory: {name}");
            foreach (var file in Directory.GetFiles(name))
            {
                if (file.IsVideoFile())
                {
                    DetectFileOrUrl(file, to, k);
                }
            }
        }

        static void Main(string[] args)
        {
            var k = 0.1;
            var to = string.Empty;
            var showHelp = false;

            var set = new OptionSet() {
{ "k|koef=", "Detection relative area. Default is 0.1", (double v) => k = v },
{ "o|out=", @"Out path. Default is fileDir or currentDir", v => to = v },
{ "h|help",  "Show this message and exit", v => showHelp = v != null },
{ "e|emgu=",  "Path to EmguCV dlls", v => Directory.SetCurrentDirectory(v) },
            };
            
            try
            {
                var names = set.Parse(args);
                if (showHelp || names.Count < 1)
                {
                    Console.WriteLine(@"This program detect all moves in video.
Example of usage:
    detector -out=output file.mp4
Arguments:");
                    set.WriteOptionDescriptions(Console.Out);
                    return;
                }

                foreach (var name in names)
                {
                    if (Directory.Exists(name))
                    {
                        DetectDirectory(name, to, k);
                    }
                    else
                    {
                        DetectFileOrUrl(name, to, k);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($@"Exception: {e.Message}{e.StackTrace}
Try `detector --help' for more information.");
            }
        }
    }
}
