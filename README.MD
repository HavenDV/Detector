# MotionDetector [![Language](https://img.shields.io/badge/language-C%23-blue.svg?style=flat-square)](https://github.com/HavenDV/MotionDetector/search?l=C%23)
 
Branches
========

master (stable)
---------------
Travic CI(Mono) | AppVeyor
--------------- | -------------
[![Build Status](https://api.travis-ci.org/HavenDV/MotionDetector.svg?branch=master)](https://travis-ci.org/HavenDV/MotionDetector) | [![Build status](https://ci.appveyor.com/api/projects/status/dhj18w01i7d753g4/branch/master?svg=true)](https://ci.appveyor.com/project/HavenDV/MotionDetector/branch/master)

# Requirements
+ msvc14+/mono4.6.2+

# Contacts
* [mail](mailto:havendv@gmail.com)

MotionDetector - This program detect all moves in video

# Linux

Preinstall:
```
sudo apt install mono-complete git nuget
```

Launch:
```
rm -r -f MotionDetector
git clone https://github.com/HavenDV/MotionDetector
nuget restore MotionDetector/MotionDetector.sln
xbuild MotionDetector/MotionDetector.sln /p:Configuration=Release
MotionDetector/MotionDetector/bin/Release/Detector.exe
```
