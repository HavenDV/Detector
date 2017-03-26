namespace MotionDetector.Tests
{
    using NUnit.Framework;
    using MotionDetector;

    [TestFixture]
    public class StringExtensions_Tests
    {
        [Test]
        public void IsUrl()
        {
            Assert.IsTrue("rtsp://mpv.cdn3.bigCDN.com:554/bigCDN/definst/mp4:bigbuckbunnyiphone_400.mp4".IsUrl());

            Assert.IsFalse("bigbuckbunnyiphone_400.mp4".IsUrl());
        }

        [Test]
        public void IsVideoFile()
        {
            Assert.IsTrue("bigbuckbunnyiphone_400.mp4".IsVideoFile());
            Assert.IsTrue("bigbuckbunnyiphone_400.Mp4".IsVideoFile());
            Assert.IsTrue("bigbuckbunnyiphone_400.MP4".IsVideoFile());
            Assert.IsTrue("child/bigbuckbunnyiphone_400.mp4".IsVideoFile());
            Assert.IsTrue("child/bigbuckbunnyiphone_400.Ts".IsVideoFile());
            Assert.IsTrue("child/bigbuckbunnyiphone_400.ts".IsVideoFile());

            Assert.IsFalse("child/bigbuckbunnyiphone_400.jpg".IsVideoFile());
        }
    }
}
