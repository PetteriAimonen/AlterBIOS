# This program combines two binary files. Any nonzero bytes in first file
# overwrite the values from second file.

import sys

if len(sys.argv) != 4:
        print 'Usage: python combine.py patch.bin original.bin output.bin'
        sys.exit(1)
        
patch = open(sys.argv[1], 'rb').read()
original = open(sys.argv[2], 'rb').read()
output = open(sys.argv[3], 'wb')

for i, c in enumerate(original):
    if i < len(patch) and ord(patch[i]) != 0:
        c = patch[i]
        
    output.write(c)
