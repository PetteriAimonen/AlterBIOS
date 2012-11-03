import json
import sys

if len(sys.argv) != 2:
    print 'Usage: python make_jump_table.py list_of_functions.json'
    sys.exit(1)

functions = json.load(open(sys.argv[1])).items()
functions.sort(key = lambda a: a[1])

print '\t@ Generated code - see make_jump_table.py'
print
print '\t.thumb'
print '\t.syntax unified'
print '\t.text'
print
print '.word 0xA17%04x' % max([i for f,i in functions])
print

for function, index in functions:
    if index < 0:
        continue
    
    print '\t.org 0x%08x' % (index * 4)
    print '\tb.w ' + function
    print
