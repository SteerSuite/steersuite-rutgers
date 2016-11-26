import sys

def parseArg(arg):
    outargs = []
    args = arg.split(",")
    for _arg in args:
        _args = _arg.split("=")
        if len(_args) == 1:
            _args.append("") # In the case of no second value
        outargs.append(_args)
    return {key: value for (key, value) in outargs}
    
print "ArgParseTest: " + sys.argv[1]

arg = sys.argv[1]

args = parseArg(arg)
print args
    