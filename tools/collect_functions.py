# This scripts collects the names of all functions in a header file and
# assigns them identifiers.

from collections import OrderedDict
from pycparser import CParser
import pycparser.c_ast as c_ast

def find_functions(text):
    '''Takes in preprocessed C sourcecode.
    Returns a list of function names like ['printf', 'sprintf', 'snprintf'].
    '''
    tree = CParser().parse(text, "")
    results = []
    
    for item in tree.ext:
        if isinstance(item, c_ast.Decl) and isinstance(item.type, c_ast.FuncDecl):
            results.append(item.name)
    
    return results

def update_dict(dictionary, functions):
    '''Takes in list of function names and a dictionary {name: id}.
    Assigns new identifiers to any functions not already in dictionary.
    '''
    for name in functions:
        if name not in dictionary:
            new_id = max(dictionary.values() + [0]) + 1
            dictionary[name] = new_id
            print "New function:", new_id, name
    
if __name__ == '__main__':
    import sys
    import json
    
    if len(sys.argv) != 2:
        print 'Usage: cpp file.h | python collect_functions.py list_of_functions.json'
    
    functions = find_functions(sys.stdin.read())
    
    if len(sys.argv) == 2:
        d = json.load(open(sys.argv[1]), object_pairs_hook = OrderedDict)
        update_dict(d, functions)
        json.dump(d, open(sys.argv[1], 'w'), indent = 4)
    else:
        for function in functions:
            print function
