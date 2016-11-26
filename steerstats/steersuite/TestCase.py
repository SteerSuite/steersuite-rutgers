import xml.etree.ElementTree as ElementTree
import xml.etree as etree
import xml.dom.minidom
import copy
import random
import cProfile, pstats, io
from Agent import Agent
from AxisAlignedBox import AxisAlignedBox
import sys
sys.path.append("../")
from tools.gameobjects.vector3 import Vector3 as vector


class TestCase(object):
    _Element_Tree=""
    _xmlns="{http://www.magix.ucla.edu/steerbench}"
    _worldbounds="worldBounds"
    _default_empty_Testcase = """<!--
  Copyright (c) 2009 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
  See license.txt for complete license.
-->

<SteerBenchTestCase xmlns="http://www.magix.ucla.edu/steerbench"
                    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                    xsi:schemaLocation="http://www.magix.ucla.edu/steerbench 
                                        TestCaseSchema.xsd">
</SteerBenchTestCase>"""

    _namespaces = {'xmlns' : 'http://www.magix.ucla.edu/steerbench',
                   '' : 'http://www.magix.ucla.edu/steerbench'}
# There is a bug with the way I use the namespace in this Class.
    def __init__(self, Element_Tree=""):
        if Element_Tree == "":
            # eliminate namespace name in xml file
            self._Element_Tree = ElementTree.ElementTree(element=ElementTree.fromstring(self._default_empty_Testcase))
            ElementTree.register_namespace("","http://www.magix.ucla.edu/steerbench")
        else:
            self._Element_Tree = ElementTree.ElementTree(element=ElementTree.fromstring(Element_Tree))
            ElementTree.register_namespace("","http://www.magix.ucla.edu/steerbench")
            
            
    #after a lot of stress the {http://www.magix.ucla.edu/steerbench} (xmlns/namespace)
    def getNumAgents(self):
        num_agents = 0
        agent_regions = self._Element_Tree.findall("./"+self._xmlns+"agentRegion/")
        # print agent_regions
        # print agent_regions.find("./numAgents/")
        for region in agent_regions:
            # print (region.tag)
            # print (region.text)
            if region.tag == self._xmlns+"numAgents":
                num_agents = num_agents + int(region.text)
        num_agents = num_agents + len(self._Element_Tree.findall("./"+self._xmlns+"agent"))
        # num_agents = num_agents + len(self._Element_Tree.findall("./agent"))
        return num_agents

    def getAgents(self):
        agents=[]
        
        for element in self._Element_Tree.findall("./"+self._xmlns+"agent"):
            agents.append(self.convertElementToAgent(element))
            
        return agents

    def randPositionIn(self, xmin, xmax, zmin, zmax, radius):
        """
            Returns a vector with position inside bounds
        """
        xmin = xmin + radius
        xmax = xmax - radius
        zmin = zmin + radius
        zmax = zmax - radius
        tmp_v = vector(random.uniform(xmin, xmax),0,random.uniform(zmin, zmax))
        return tmp_v
    
    def randPositionInWithoutColisions(self, xmin, xmax, zmin, zmax, radius):
        """
            Returns a vector with position inside bounds
        """
        tryTimes = 1000
        xmin = xmin + radius
        xmax = xmax - radius
        zmin = zmin + radius
        zmax = zmax - radius
        agents = self.getAgents()
        agent = Agent()
        agent.set_radius(0.5)
        badLoc=False
        # random.seed(seed)
        for i in range(tryTimes):
            tmp_v = vector(random.uniform(xmin, xmax),0,random.uniform(zmin, zmax))
            # print tmp_v
            agent.set_position(tmp_v)
            for tmp_agent in agents:
                if agent.overlaps(tmp_agent):
                    # agent overlaps another generate a new position
                    badLoc=True
                    break
            if badLoc:
                badLoc=False
                continue
            else:
                # made it though test with other agents so no overlapping
                break
        if i >= (tryTimes-1):
            print "Gave up trying to find a placement without collisions"
        return tmp_v
    
    def convertElementToAgent(self, element):
        agent = Agent()
        ic = element.find("./"+self._xmlns+"initialConditions")
        position = ic.find("./"+self._xmlns+"position")
        px = float(position.find("./"+self._xmlns+"x").text)
        py = float(position.find("./"+self._xmlns+"y").text)
        pz = float(position.find("./"+self._xmlns+"z").text)
        
        direction = ic.find("./"+self._xmlns+"direction")
        dx = float(direction.find("./"+self._xmlns+"x").text)
        dy = float(direction.find("./"+self._xmlns+"y").text)
        dz = float(direction.find("./"+self._xmlns+"z").text)
        
        radius = float(ic.find("./"+self._xmlns+"radius").text)
        
        speed = float(ic.find("./"+self._xmlns+"speed").text)

        pos = vector(px,py,pz)
        dir = vector(dx, dy, dz)
        agent.set_position(pos)
        agent.set_direction(dir)
        agent.set_radius(radius)
        agent.set_speed(speed)
        return agent 
    
    def removeAgents(self):
        agent_regions = self._Element_Tree.findall("./"+self._xmlns+"agentRegion/")
        for region in agent_regions:
            # print (region.tag)
            self._Element_Tree.getroot().remove(region)
        
        for agent in self._Element_Tree.findall("./"+self._xmlns+"agent"):
            # print "removing " + str(agent)
            self._Element_Tree.getroot().remove(agent)
    
    def getNumObstacles(self):
        obstacles = self._Element_Tree.findall("./"+self._xmlns+"obstacle")
        return len(obstacles)
    
    # prints a lot prettier now. 
    # NOTE: editors have trouble editing extreamly large lines in files.
    def getTextPretty(self):
        return xml.dom.minidom.parseString(
                                    ElementTree.tostring(self._Element_Tree.getroot(),
                                    encoding="us-ascii", method="xml")).toprettyxml()
    
    def getText(self):
        return ElementTree.tostring(self._Element_Tree.getroot(), encoding="us-ascii", method="xml")
    
    def addObstacle(self, xmin, xmax, ymin, ymax, zmin, zmax):
        # creates a new subelement
        obstacle = createSubElement(self, self._Element_Tree.getroot(),"obstacle")
        
        createSubElement(self, obstacle, "xmin").text = str(xmin)
        createSubElement(self, obstacle, "xmax").text = str(xmax)
        createSubElement(self, obstacle, "ymin").text = str(ymin)
        createSubElement(self, obstacle, "ymax").text = str(ymax)
        createSubElement(self, obstacle, "zmin").text = str(zmin)
        createSubElement(self, obstacle, "zmax").text = str(zmax)
        # _xmin.text = str(xmin)
        
    def addCircleObstacle(self, x, y, z, radius, height):
        """
        <circleObstacle>
          <radius>2</radius>
          <height>5</height>
          <position><x>0</x> <y>0</y> <z>5</z></position>
          
        </circleObstacle>
        """
        # creates a new subelement
        obstacle = createSubElement(self, self._Element_Tree.getroot(),"circleObstacle")
        
        position = createSubElement(self, obstacle, "position")
        createSubElement(self, position, "x").text = str(x)
        createSubElement(self, position, "y").text = str(y)
        createSubElement(self, position, "z").text = str(z)
        
        createSubElement(self, obstacle, "radius").text = str(radius)
        createSubElement(self, obstacle, "height").text = str(height)
        
    def createSubElement(self, parent, text):
        return ElementTree.SubElement(parent,self.getNameSpace()+text)
    
    def addAgent(self, name="blah", radius=0.5, agentPos=[], directionV=[1, 0, 0],
                  speed=0, targetLocationV=[], desiredSpeed=1.25, timeDuration=1000.0):
        agent = createSubElement(self, self._Element_Tree.getroot(), "agent")

        createSubElement(self, agent, "name").text = str(name)
        initialConditions = createSubElement(self, agent, "initialConditions")
        createSubElement(self, initialConditions, "radius").text = str(radius)
        
        position = createSubElement(self, initialConditions, "position")
        createSubElement(self, position, "x").text = str(agentPos[0])
        createSubElement(self, position, "y").text = str(agentPos[1])
        createSubElement(self, position, "z").text = str(agentPos[2])
        
        direction = createSubElement(self, initialConditions, "direction")
        createSubElement(self, direction, "x").text = str(directionV[0])
        createSubElement(self, direction, "y").text = str(directionV[1])
        createSubElement(self, direction, "z").text = str(directionV[2])
        
        createSubElement(self, initialConditions, "speed").text = str(speed)
        
        goalSequence = createSubElement(self, agent, "goalSequence")
        seekStaticTarget = createSubElement(self, goalSequence,  "seekStaticTarget")
        
        targetLocation = createSubElement(self, seekStaticTarget, "targetLocation")
        createSubElement(self, targetLocation, "x").text = str(targetLocationV[0])
        createSubElement(self, targetLocation, "y").text = str(targetLocationV[1])
        createSubElement(self, targetLocation, "z").text = str(targetLocationV[2])
        
        createSubElement(self, seekStaticTarget, "desiredSpeed").text = str(desiredSpeed)
        createSubElement(self, seekStaticTarget, "timeDuration").text = str(timeDuration)
        return 0
    
    def addHeader(self, name, version, xmin, xmax, ymin, ymax, zmin, zmax):
        # example
        """  <header>
                <version>1.0</version>
                <name>BigGameHunters</name>
                <worldBounds>
                  <xmin>-250</xmin>
                  <xmax>250</xmax>
                  <ymin>0</ymin>
                  <ymax>0</ymax>
                  <zmin>-250</zmin>
                  <zmax>250</zmax>
                </worldBounds>
              </header>"""

        # remove any header if it exists already        
        for header in self._Element_Tree.findall('./'+self._xmlns+'header'):
            self._Element_Tree.getroot().remove(header)
            
        
        header = self.createSubElement(self._Element_Tree.getroot(), "header")
        self.createSubElement(header, "version").text = str(version)
        self.createSubElement(header, "name").text = str(name)
        worldBounds = self.createSubElement(header, "worldBounds")
        self.createSubElement(worldBounds, "xmin").text = str(xmin)
        self.createSubElement(worldBounds, "xmax").text = str(xmax)
        self.createSubElement(worldBounds, "ymin").text = str(ymin)
        self.createSubElement(worldBounds, "ymax").text = str(ymax)
        self.createSubElement(worldBounds, "zmin").text = str(zmin)
        self.createSubElement(worldBounds, "zmax").text = str(zmax)
        
        
    def getWorldXmin(self):
        search = "./"+self._xmlns+"header/"+self._xmlns+self._worldbounds+'/'+self._xmlns+'xmin'
        # search = "./header/"+self._worldbounds+'/xmin'
        xmin = self._Element_Tree.findall(search, namespaces=self._namespaces)[0].text
        # print "xmin: " + str(xmin)
        return float(xmin)
        
    def getWorldXmax(self):
        search = "./"+self._xmlns+"header/"+self._xmlns+self._worldbounds+'/'+self._xmlns+'xmax'
        xmin = self._Element_Tree.findall(search)[0].text
        # print "xmin: " + str(xmin)
        return float(xmin)       
  
    def getWorldZmin(self):
        search = "./"+self._xmlns+"header/"+self._xmlns+self._worldbounds+'/'+self._xmlns+'zmin'
        xmin = self._Element_Tree.findall(search)[0].text
        # print "xmin: " + str(xmin)
        return float(xmin)
        
    def getWorldZmax(self):
        search = "./"+self._xmlns+"header/"+self._xmlns+self._worldbounds+'/'+self._xmlns+'zmax'
        xmin = self._Element_Tree.findall(search)[0].text
        # print "xmin: " + str(xmin)
        return float(xmin)    
    
    def getNameSpace(self):
        return self._xmlns
    
def createSubElement(thing, parent, text):
    return ElementTree.SubElement(parent,thing.getNameSpace()+text)    