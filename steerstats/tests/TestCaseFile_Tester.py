import sys
import unittest

sys.path.append("../")

from steersuite.TestCase import TestCase

file_name = sys.argv[1]

xmlData = open(file_name, "r").read()
test_cas = TestCase(xmlData)
    
# print "****" + str(test_cas._Element_Tree.findall("./agent/name"))

#class TestCase_Tester(unittest.TestCase):
class TestCase_Tester():

    def test_agent_count(self):
        print test_cas.getNumAgents()
        # self.assertEqual( test_cas.getNumAgents(), 4)
        
    def test_obstacle_count(self):
        print test_cas.getNumObstacles()
        # self.assertEqual( test_cas.getNumObstacles(), 29)
      
    def test_header_world_xmin(self):
        print test_cas.getWorldXmin()  
        

test_reader = TestCase_Tester()
print test_cas.getText()
test_reader.test_agent_count()
test_reader.test_obstacle_count()
test_reader.test_header_world_xmin()
