#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuitedb import AlgorithmData
from steersuitedb.Composite1Benchmark import Composite1Benchmark
from steersuitedb.ConnectionManager import ConnectionManager
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuite.LogParser import LogParser
from steersuite.TestCase import TestCase


con = None
ccID=0

class CcAI_Tester(unittest.TestCase):

    def test_Insert(self):
        try:
             
            conM = ConnectionManager()
            cur = conM.getCursor()    
        
        
            ldat = {}
            benchmarkLogFile = open("data/test.log", "r")
            
            rand_seed = benchmarkLogFile.readline()
            
            testcaseFile = open("data/pprPlansToNotGoal.xml", "r")
            testcase = testcaseFile.read()
            testcase_ = TestCase(testcase)
            
            lparser = LogParser()
            logData = lparser.parseLog(benchmarkLogFile)
            
            benchmark = Benchmark()
            benchmarkIndex = benchmark.getBenchmarkIndex(cur, "composite02")
            
            algorithm = Algorithm()
            algorithmIndex = algorithm.getAlgorithmIndex(cur, "pprAI")
            
            ldat['algorithm_data_id']=1
            ldat['test_comments']="insertion test"
            ldat['benchmark_id']=benchmarkIndex
            ldat['test_case']=testcase
            ldat['test_status']=0
            ldat['scenario_group']=1
            __test_case = TestCase(testcase)
            # // TODO
            ldat['num_agents']=__test_case.getNumAgents()
            ldat['num_obstacles']=__test_case.getNumObstacles()
            
            ldat['num_agents'] = testcase_.getNumAgents()
            ldat['num_obstacles'] = testcase_.getNumObstacles()
            
            testcase = "blah blahbidy blah"
            
            test_status = 0
            
            scenario_group = 3
            
            # print logData[0]
            
            for dataLine in logData:
                # print dataLine
                ad = Composite1Benchmark()
                _data = dict(dataLine.items() + ldat.items())
                # print _data
                ad.setValuesFromDict(_data)
                
                # print ad
        
                benchmark_data_id = ad.insertBenchmark2(cur)
                
                other_ad = ad.getBenchmarkData(cur, benchmark_data_id)
                self.assertEqual( ad._time,
                               str(other_ad._time) )
                self.assertEqual( ad._frames,
                               str(other_ad._frames) )
                self.assertEqual( ad._rand_calls,
                               str(other_ad._rand_calls) )
            
        #    ad.setAlgorithmReff("pprAI")
            # ad.insertPprAIData2(cur)
            
        
        
        except psycopg2.DatabaseError, e:
            print 'Error Composite 1 Benchmark-test: %s' % e    
            sys.exit(1)
            
            
        finally:
            
            if con:
                con.close() 
            
            
