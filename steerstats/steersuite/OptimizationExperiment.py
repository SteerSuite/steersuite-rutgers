import xml.etree.ElementTree as ElementTree
from OptimizationParameter import OptimizationParameter

class OptimizationExperiment():
    _Element_Tree=""
    _xmlns="{http://www.magix.ucla.edu/steerbench}"

    def __init__(self, Element_Tree=""):
        self._Element_Tree = ElementTree.fromstring(Element_Tree)
        
    def parseExperimentXML(self):
        parameterElements = self._Element_Tree.findall("./ai_config/parameter")
        parameters = []
        for paramElement in parameterElements:
            parameters.append(self._parseParameter(paramElement))
        return parameters
    
    def _parseParameter(self, element):
        min_ = float(element.find("./min").text)
        max_ = float(element.find("./max").text)
        discretization = float(element.find("./discretization").text)
        original = float(element.find("./original").text)
        name = element.get("name")
        type = element.get("type")
        Op = OptimizationParameter(name=name, min_=min_, max_=max_, original=original
                                   , descretization=discretization, type=type)
        return Op