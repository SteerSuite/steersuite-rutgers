import random
import unittest
import sys
sys.path.append("../")
from steerstats.SteerStats import SteerStats 
from steersuite.SteerStatsOptions import getOptions

class SocialForcesTester(unittest.TestCase):

    def setUp(self):
        self._ai = 'sfAI'
        self.seq = list(range(10))

    @unittest.skip("demonstrating skipping")
    def test_skipped(self):
        self.fail("shouldn't happen")
        
    def test_one_scenario(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			self._ai,
			'--skipInsert',
			'-c',
			'--numScenarios',
			'1'
			]
		
		options = getOptions(_args)
		
		steerStats = SteerStats(options)
		results = steerStats.RunStats()
		# print "Length of result: " + (str(len(results)))
		self.assertEqual(1, len(results))
		
    def test_ten_scenarios(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			self._ai,
			'--skipInsert',
			'-c',
			'--numScenarios',
			'10'
			]
		
		options = getOptions(_args)
		
		steerStats = SteerStats(options)
		results = steerStats.RunStats()
		# print "Length of result: " + (str(len(results)))
		self.assertEqual(10, len(results))
		
    def test_ten_random_scenarios(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			self._ai,
			'--skipInsert',
			'-c',
			'--numScenarios',
			'10'
			]
		
		options = getOptions(_args)
		
		steerStats = SteerStats(options)
		results = steerStats.RunStats()
		# print "Length of result: " + (str(len(results)))
		self.assertEqual(10, len(results))
		
    def test_onehundred_scenarios(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			self._ai,
			'--skipInsert',
			'-c',
			'--numScenarios',
			'100'
			]
		
		options = getOptions(_args)
		
		steerStats = SteerStats(options)
		results = steerStats.RunStats()
		# print "Length of result: " + (str(len(results)))
		self.assertEqual(100, len(results))
        
    def test_3_squeeze_scenarios(self):
        # _args="--ai=sfAI"
        _args=[
            '--ai',
            self._ai,
            '--skipInsert',
            '-c',
            '--numScenarios',
            '1',
            '--scenarioDIR',
            'data/scenarios/testcases/'
            ]
        
        options = getOptions(_args)
        
        steerStats = SteerStats(options)
        results = steerStats.RunStats()
        # print "Length of result: " + (str(len(results)))
        self.assertEqual(1, len(results))

