AlterBIOS for DSO Quad
======================

The DSO Quad original BIOS has some limitations: mainly, it corrupts the filesystem quite often.
Also, it doesn't include many features, some of which could be very useful to many applications.

The original BIOS is open source, but requires IAR V5.0 to build.
[GCC port](https://github.com/neilstockbridge/dsoquad-BIOS) is not yet functional.

AlterBIOS approaches this problem from another direction. It installs into the extra flash on
the processor, and patches the BIOS entrypoints to point into AlterBIOS instead.

Download binaries [here](http://koti.kapsi.fi/~jpa/dsoquad/).

Update 25/06/2021:
- Modified Makefile to call python2
- moved from SYS_B152.HEX to SYS_B164.HEX as base system
- patched alterbios.c to use the correct sector size according to the Flash disk size.

Successfully tested on DSO203 V2.72 8Mb both PA1_V113 and Wildcat5.6.
