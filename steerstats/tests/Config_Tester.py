import sys
import unittest

sys.path.append("../")

from steersuite.Configuration import Configuration

path_to_config = sys.argv[1]
print "Testing config: " + str(path_to_config)

xmlData = open(path_to_config, "r").read()

# config = Config(xmlData)
config = Configuration(xmlData)
class Config_Tester(unittest.TestCase):
    
    def test_getFPS(self):
        self.assertEqual( config.getFPS(), 24 )
        
    def test_grid_sizeX(self):
        self.assertEqual(config.getGridSizeX(), 199)
        
    def test_grid_sizez(self):
        self.assertEqual(config.getGridSizeZ(), 200)
        
    def test_grid_cells_X(self):
        self.assertEqual(config.getGridCellsX(), 440)
        
    def test_grid_cells_z(self):
        self.assertEqual(config.getGridCellsZ(), 441)
        
   # def test_tostring(self):
   #     self.assertEqual(config.getSource(), xmlData)
        
# print config.getSource()
    # print config._Element_Tree.findall("./engine")
            
# print "Grid Dasebase size X:" + str(config.getGridSizeX())
#    print "Grid Dasebase size Z:" + str(config.getGridSizeZ())
    
#    print "Grid Dasebase Cells X:" + str(config.getGridCellsX())
#    print "Grid Dasebase Cells Z:" + str(config.getGridCellsZ())

# print config.getParameters()
# print config.getParameterString()