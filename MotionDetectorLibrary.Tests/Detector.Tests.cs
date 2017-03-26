namespace MotionDetectorLibrary.Tests
{
    using System;
    using System.IO;
    using System.Reflection;
    using NUnit.Framework;

    [TestFixture]
    public class UnitTest1
    {
        private string GetFullPath(string filename) =>
            Path.Combine(
                Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(
                    Assembly.GetExecutingAssembly().Location))
                ),
                "TestFiles", filename);

        private string GetEmguCVDllDir()
        {
            return Path.Combine(
                Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(
                        Assembly.GetExecutingAssembly().Location))
                )), "packages", "EmguCV.3.1.0.1", "build", "native", "x86");
        }

        [Test]
        public void Detector_Rtsp()
        {
            Directory.SetCurrentDirectory(GetEmguCVDllDir());
            Assert.DoesNotThrow(
                () => Detector.Detect("rtsp://mpv.cdn3.bigCDN.com:554/bigCDN/definst/mp4:bigbuckbunnyiphone_400.mp4", Environment.CurrentDirectory, 0.1, true)
                );
        }

        [Test]
        public void Detector_Mp4()
        {
            Directory.SetCurrentDirectory(GetEmguCVDllDir());
            Assert.DoesNotThrow(
                () => Detector.Detect(GetFullPath("test.mp4"), Environment.CurrentDirectory, 0.1, true)
                );
        }
    }
}
