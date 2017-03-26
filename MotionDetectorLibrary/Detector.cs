namespace MotionDetectorLibrary
{
    using System;
    using System.Drawing;
    using System.IO;
    using Emgu.CV;
    using Emgu.CV.CvEnum;
    using Emgu.CV.Structure;
    using Emgu.CV.VideoSurveillance;

    public static class Detector
    {
        public static string MsToTimeString(double ms)
        {
            var s = ((int)Math.Floor(ms / 1000)) % 60;
            var m = ((int)Math.Floor(ms / 1000 / 60)) % 60;
            var h = ((int)Math.Floor(ms / 1000 / 60 / 60)) % 24;

            return $"{h}-{m}-{s}";
        }

        public static void Detect(string file, string to, double k)
        {
            var capture = new Capture(file);
            var frameCount = capture.GetCaptureProperty(CapProp.FrameCount);
            var subtractor = new BackgroundSubtractorMOG2();

            var i = 0;
            while (true)
            {
                ++i;
                var mat = capture.QueryFrame();
                if (i % 30 != 0)
                {
                    continue;
                }

                if (mat.IsEmpty)
                {
                    break;
                }

                var mask = new Mat();
                subtractor.Apply(mat, mask);

                var outMat = mat.Clone();
                var s = mat.Size.Height * mat.Size.Width;
                var count = CvInvoke.CountNonZero(mask);
                var isDetected = i > 60 && count > k * s;

                if (isDetected)
                {
                    CvInvoke.PutText(outMat, "Detected", new Point(50, 50), FontFace.HersheyComplex, 1.0, new MCvScalar(0,0, 255));
                    var time = capture.GetCaptureProperty(CapProp.PosMsec);
                    var outPath = Path.Combine(to, MsToTimeString(time)) + ".jpg";
                    CvInvoke.Imwrite(outPath, outMat);
                    Console.WriteLine($"Save file as: {outPath}");
                }
                if (i % 1000 == 0)
                {
                    Console.WriteLine($"Progress: {(100.0 * i) / frameCount}%");
                }
                CvInvoke.WaitKey(1);
            }
        }
    }
}
