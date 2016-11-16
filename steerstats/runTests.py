
from unit_tests import HTMLTestRunner
from unit_tests.SocialForcesTester import SocialForcesTester
from unit_tests.ORCATester import ORCATester
from unit_tests.PPRTester import PPRTester
import unittest


suite_sf = unittest.TestLoader().loadTestsFromTestCase(SocialForcesTester)
suite_orca = unittest.TestLoader().loadTestsFromTestCase(ORCATester)
suite_ppr = unittest.TestLoader().loadTestsFromTestCase(PPRTester)

outfile = open("TestingReport.html", "w")
runner = HTMLTestRunner.HTMLTestRunner(
				stream=outfile,
				title='SteerSuite Test Report',
				description='Unit tests for SteerSuite crowd simulation framework.'
				)

runner.run(suite_sf)
runner.run(suite_orca)
runner.run(suite_ppr)