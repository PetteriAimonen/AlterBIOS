AlterBIOS for DSO Quad
======================

The DSO Quad original BIOS has some limitations: mainly, it corrupts the filesystem quite often.
Also, it doesn't include many features, some of which could be very useful to many applications.

The original BIOS is open source, but requires IAR V5.0 to build.
[GCC port](https://github.com/neilstockbridge/dsoquad-BIOS) is not yet functional.

AlterBIOS approaches this problem from another direction. It installs into the extra flash on
the processor, and patches the BIOS entrypoints to point into AlterBIOS instead.

Download binaries [here](http://koti.kapsi.fi/~jpa/dsoquad/).
