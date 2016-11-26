import xml.etree.ElementTree as ET

libraryDirectory = '../../build/lib/'
class Configuration(object):
    _Element_Tree=""
    _xmlns=""
    _steerStats="steerstats"
    _default_empty_Testcase = """<!--
  Copyright (c) 2009-2015 Glen Berseth, Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
  See license.txt for complete license.
-->

<SteerSimOptions >
</SteerSimOptions>"""


    def __init__(self, Element_Tree=""):
        # print Element_Tree
        self._Element_Tree = ET.fromstring(Element_Tree)
        # don't ask
        # self._source = str(Element_Tree)
        
        
    def getFPS(self):
        fpsNode = self._Element_Tree.findall("./engine/fixedFPS")
        return int(fpsNode[0].text)
    
    def getGridSizeZ(self):
        return int(self._Element_Tree.findall("./gridDatabase/sizeZ")[0].text)
    
    def getGridSizeX(self):
        return int(self._Element_Tree.findall("./gridDatabase/sizeX")[0].text)

    def getGridCellsZ(self):
        return int(self._Element_Tree.findall("./gridDatabase/numCellsZ")[0].text)
    
    def getGridCellsX(self):
        return int(self._Element_Tree.findall("./gridDatabase/numCellsX")[0].text)
    
    def getParameters(self):
        search = "./"+self._xmlns+"modules/"+self._xmlns+self._steerStats
        # search = "./header/"+self._worldbounds+'/xmin'
        xmin = self._Element_Tree.findall(search)[0]
        parametersDict={}
        for item in xmin:
            # print item.tag , item.attrib, item.text
            parametersDict[str(item.tag)] = item.text
        # print "xmin: " + str(xmin)
        return parametersDict
    
    def getMetricWeights(self):
        search = "./"+self._xmlns+"modules/"+self._xmlns+self._steerStats+'/metricWeights'
        # search = "./header/"+self._worldbounds+'/xmin'
        xmin = self._Element_Tree.findall(search)[0]
        parametersDict={}
        for item in xmin:
            # print item.tag , item.attrib, item.text
            parametersDict[str(item.tag)] = item.text
        # print "xmin: " + str(xmin)
        return parametersDict
    
    def getProcessingEvents(self):
        search = "./"+self._xmlns+"modules/"+self._xmlns+self._steerStats+'/processingEvents'
        # search = "./header/"+self._worldbounds+'/xmin'
        xmin = self._Element_Tree.findall(search)[0]
        parametersDict={}
        for item in xmin:
            # print item.tag , item.attrib, item.text
            parametersDict[str(item.tag)] = item.text
        # print "xmin: " + str(xmin)
        return parametersDict
    
    def getParameterString(self):
        params = self.getParameters()
        out=""
        for key in params:
            if params[key] is None:
                out = out + "--"+str(key)+" "
            else:
                out = out + "--"+str(key)+"="+str(params[key])+" "

        return out[:-1]
        
    #def getSource(self):
    #    return ET.tostring(self._Element_Tree.getroot())
        # return self._source 
    
    