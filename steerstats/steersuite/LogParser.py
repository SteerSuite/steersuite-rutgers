import cProfile, pstats, io

class LogParser(object):
# log should be an open file object that is readable
    def parseLog(self, log):
        """
            Always make sure to read and discard the random seed that is at the
            top of benchmark log files.
        """
        dictList = []
        keys = []
        # need to remove newlines
        keys=self.parseLine(log.readline().rstrip('\n').rstrip('\r'))
        for line in log:
            #if n == 1:
                #keys=self.parseLine(line)
            #if n > 1: 
                # list.append(self.parseLine(line))
            dict = {key: value for (key, value) in zip(keys, self.parseLine(line.rstrip('\n')))}
            dictList.append(dict)
            # n = n + 1
            
        return dictList
        
    def parseStringLog(self, log):
        
        dictList = []
        keys = []
        # need to remove newlines
        keys=self.parseLine(log[0].rstrip('\n'))
        for line in log[1:]:
            #if n == 1:
                #keys=self.parseLine(line)
            #if n > 1: 
                # list.append(self.parseLine(line))
            dict = {key: value for (key, value) in zip(keys, self.parseLine(line.rstrip('\n')))}
            dictList.append(dict)
            # n = n + 1
            
        return dictList
        
    def parseLine(self, line):
        parts = line.split(" ");
        return parts
    
    
    def combine(self, benchmarkData, aiData):
        
        out = []
        for benchmarkDat, aiDat in zip(benchmarkData,aiData):
            out.append(dict(benchmarkDat.items() + aiDat.items()))
        return out
        
    def parsePostgres(self, text):
        parts = text.split("\n")
        entries = []
        for part in parts:
            part = part.lstrip(" ")# removes all ' ' characters
            if part.isdigit():
                entries.append(part)
        return entries

    