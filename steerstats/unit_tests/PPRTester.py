import random
import unittest
import sys
sys.path.append("../")
from steerstats.SteerStats import SteerStats 
from steersuite.SteerStatsOptions import getOptions

class PPRTester(unittest.TestCase):

    def setUp(self):
        self.seq = list(range(10))

    @unittest.skip("demonstrating skipping")
    def test_skipped(self):
        self.fail("shouldn't happen")
        
    def test_one_scenario(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			'pprAI',
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
			'pprAI',
			'--skipInsert',
			'-c',
			'--numScenarios',
			'10',
			'-v'
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
			'pprAI',
			'--skipInsert',
			'-c',
			'--numScenarios',
			'10',
			'-v'
			]
		
		options = getOptions(_args)
		
		steerStats = SteerStats(options)
		results = steerStats.RunStats()
		# print "Length of result: " + (str(len(results)))
		self.assertEqual(10, len(results))
    
    # @unittest.skip("demonstrating skipping")
    def test_onehundred_scenarios(self):
		# _args="--ai=sfAI"
		_args=[
			'--ai',
			'pprAI',
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

    def test_shuffle(self):
        # make sure the shuffled sequence does not lose any elements
        random.shuffle(self.seq)
        self.seq.sort()
        self.assertEqual(self.seq, list(range(10)))

        # should raise an exception for an immutable sequence
        self.assertRaises(TypeError, random.shuffle, (1,2,3))

    def test_choice(self):
        element = random.choice(self.seq)
        self.assertTrue(element in self.seq)

    def test_sample(self):
        with self.assertRaises(ValueError):
            random.sample(self.seq, 20)
        for element in random.sample(self.seq, 5):
            self.assertTrue(element in self.seq)
