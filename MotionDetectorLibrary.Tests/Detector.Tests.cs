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

        [Test]
        public void Detector_Rtsp()
        {
            Assert.DoesNotThrow(
                () => Detector.Detect("rtsp://mpv.cdn3.bigCDN.com:554/bigCDN/definst/mp4:bigbuckbunnyiphone_400.mp4", Environment.CurrentDirectory, 0.1)
                );
        }

        [Test]
        public void Detector_Mp4()
        {
            Assert.DoesNotThrow(
                () => Detector.Detect(GetFullPath("test.mp4"), Environment.CurrentDirectory, 0.1)
                );
        }
    }
}
