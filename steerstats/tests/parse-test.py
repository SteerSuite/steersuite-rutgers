import psycopg2

from steersuite import LogParser
from steersuitedb import Composite1Benchmark
# import steersuitedb.Composite1Benchmark

logfile = open('ppr/test.log', 'r')

lparser = LogParser.LogParser()

out = lparser.parseLog(logfile)

con = psycopg2.connect(database='steersuitedb', user='steeruser', password='steersuite') 

cur = con.cursor()   

benchmark = Composite1Benchmark.Composite1Benchmark(1,
                                                    out[2][0],
                                                    out[2][1],
                                                    out[2][2],
                                                    out[2][3],
                                                    out[2][4],
                                                    out[2][5],
                                                    out[2][6],
                                                    out[2][7],
                                                    out[2][8],
                                                    out[2][9],
                                                    out[2][10],
                                                    out[2][11],
                                                    out[2][12],
                                                    out[2][13],
                                                    out[2][14],
                                                    out[2][15],
                                                    out[2][16],
                                                    out[2][17],
                                                    out[2][18],
                                                    out[2][19],
                                                    out[2][20],
                                                    out[2][21],
                                                    out[2][22]
                                                    )
   
print out
print ":"
print out[2][3]
print benchmark._total_change_in_speed

cur.execute("Select * from nextval('algorithm_seq')")
row = cur.fetchone()
print row
print row[0]

status = benchmark.insertTest2(cur)
logfile.close()
