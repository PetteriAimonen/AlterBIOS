import json
import sys

if len(sys.argv) != 2:
    print 'Usage: python make_export.py list_of_functions.json'
    sys.exit(1)

functions = json.load(open(sys.argv[1])).items()
functions.sort(key = lambda a: a[1])

print '\t@ Generated code - see make_export.py'
print '\t@ This is the API symbols file for AlterBIOS.'
print '\t@ It is used from DSO Quad user applications to access AlterBIOS functions.'
print
print '\t.thumb'
print '\t.syntax unified'
print
print '\t.text'
print

print '\t.globl alterbios_version_tag'
print '\t. = 0x00000000'
print 'alterbios_version_tag:'
print

for function, index in functions:
    if index < 0:
        continue
    
    print '\t.globl ' + function
    print '\t.thumb_func'
    print '\t. = 0x%08x' % (index * 4 + 1)
    print function + ':'
    print
