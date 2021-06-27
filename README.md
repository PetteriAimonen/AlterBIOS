# AlterBIOS for DSO Quad

The DSO Quad original BIOS has some limitations: mainly, it corrupts the filesystem quite often.
Also, it doesn't include many features, some of which could be very useful to many applications.

The original BIOS is open source, but requires IAR V5.0 to build.
[GCC port](https://github.com/neilstockbridge/dsoquad-BIOS) is not yet functional.

AlterBIOS approaches this problem from another direction. It installs into the extra flash on
the processor, and patches the BIOS entrypoints to point into AlterBIOS instead.

Download binaries [here](http://koti.kapsi.fi/~jpa/dsoquad/).

Update 27/06/2021:

since it seems the development of SYS for these devices hardly ever will go over the current status, I changed memory map to fit alterbios in the remaining space left on SYS slot so no need to do a separate or tricky flash, just flash the ALT_F1XX.HEX/BIN in place of stock SYS file.

In this way it doesn't interfere with other projects such as Gabonator LA104 that uses unused memory address.

I patched all released SYS versions, check within compiled folder

ALT_F1XX are SYS patched+alterbios joined in a single file that fit the SYS slot.

alternatively:

ALT_B1XX are SYS patched that calls alterbios functions, it must be flashed in place of stock SYS

ALTBIOS is the overwritten SYS function file, it is flashed at the end of 256KB flash (0x08044000)

Update 25/06/2021:

- Modified Makefile to call python2
- moved from SYS_B152.HEX to SYS_B164.HEX as base system
- patched alterbios.c to use the correct sector size according to the Flash disk size.

Successfully tested on DSO203 V2.72 8Mb both PA1_V113 and Wildcat5.6.
