namespace MotionDetector
{
    using System;
    using System.IO;
    using Mono.Options;

    class Program
    {

        static void DetectFileOrUrl(string name, string to, double k)
        {
            if (!File.Exists(name))
            {
                throw new FileNotFoundException(nameof(name));
            }

            var isUrl = name.IsUrl();
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
            //Detect
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
{ "h|help",  "show this message and exit", v => showHelp = v != null },
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
                Console.WriteLine($@"Exception: {e.Message}
Try `detector --help' for more information.");
            }
        }
    }
}
