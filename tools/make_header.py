import json
import sys

if len(sys.argv) != 2:
    print 'Usage: python make_export.py list_of_functions.json'
    sys.exit(1)

functions = json.load(open(sys.argv[1])).items()
functions.sort(key = lambda a: a[1])

print '''
#ifndef __ALTERBIOS_H__
#define __ALTERBIOS_H__

extern uint32_t alterbios_version_tag;

/* Verify that the installed AlterBIOS version is compatible with this
 * API version. Return values:
 *      Success:         positive (number of exported functions)
 *      Not installed:   -1
 *      Too old version: -2
 */
static int alterbios_check()
{
    if ((alterbios_version_tag & 0xFFFF0000) != 0x0A170000)
        return -1;
    if ((alterbios_version_tag & 0x0000FFFF) < %d)
        return -2;
    return (alterbios_version_tag & 0x0000FFFF);
}

/* Initialize AlterBIOS (basically mounts the FAT filesystem) */
extern void alterbios_init();

#endif
''' % len(functions)
