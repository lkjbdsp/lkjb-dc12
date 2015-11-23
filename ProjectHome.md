# KVR Developer Challenge DC 12 entry #

The source code is available via svn. The current binaries are built using VS2008 and XCode 3.2.6. VS2005 and VS2010 projects are also included but not tested.

The binaries can be downloades from the Downloads page.

**Changelog**:

Version 1.2.2
  * fixed bug with predelay volume

Version 1.2.1
  * fixed bug with predelay time

Version 1.2.0
  * added further pitch shift algorithm (from [mda Detune](http://mda.smartelectronix.com/))
  * added optional Predelay
  * added 1/32th quantisation
  * Windows x64 support (without Dirac LE)
  * fixed version, old fxps should be loaded without fxpFixer

Version 1.1.0
  * added presets
  * added pan/balance

Version 1.0.2:
  * updated JUCE to 2.0.28
  * fixed: Audio Unit channel configuration
  * fixed: avoid oscillation due to pitching by clipping feedback signal