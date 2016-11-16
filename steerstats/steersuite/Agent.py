
import sys
sys.path.append("../")
from tools.gameobjects.vector3 import Vector3 as vector

class Agent(object):
    
    def __init__(self, position=vector(), direction=vector(), radius=0.5,
                 speed=1.33):
        self._position=position
        self._direction=direction
        self._radius=radius
        self._speed=speed

        
    def __str__(self):
        return ("agent: loc=" + str(self._position) + 
                ", dir="+str(self._direction) + 
                ", radius=" + str(self._radius) +
                ", speed=" + str(self._speed))
    def __repr__(self):
        return self.__str__()
    
    def get_position(self):
        return self._position
    def get_direction(self):
        return self._direction
    def get_radius(self):
        return self._radius
    def get_speed(self):
        return self._speed

    def set_position(self, value):
        self._position = value
    def set_direction(self, value):
        self._direction = value
    def set_radius(self, value):
        self._radius = value
    def set_speed(self, value):
        self._speed = value

    def del_position(self):
        del self._position
    def del_direction(self):
        del self._direction
    def del_radius(self):
        del self._radius
    def del_speed(self):
        del self._speed
        
    def overlaps(self, other):
        # print "do " + str(self) + " and " + str(other) + " overlap?"
        distanceV = (self.get_position() - other.get_position())
        # print "distance " + str(distanceV)
        # print "length " + str(self.get_position().lengthSquared())
        distance = distanceV.get_length()
        distanceThreshold = (self.get_radius() + other.get_radius())
        # print "distanceTreshold: " + str(distanceThreshold) + " distance: " + str(distance)
        return distanceThreshold > distance

    position = property(get_position, set_position, del_position, "position's docstring")
    direction = property(get_direction, set_direction, del_direction, "direction's docstring")
    radius = property(get_radius, set_radius, del_radius, "radius's docstring")
    speed = property(get_speed, set_speed, del_speed, "speed's docstring")
