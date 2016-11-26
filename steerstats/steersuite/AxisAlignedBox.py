

class AxisAlignedBox(object):
    
    def __init__(self, xmin, xmax, ymin, ymax, zmin, zmax):
        self._xmin=xmin
        self._xmax=xmax
        self._ymin=ymin
        self._ymax=ymax
        self._zmin=zmin
        self._zmax=zmax

    def get_xmin(self):
        return self._xmin
    def get_xmax(self):
        return self._xmax
    def get_ymin(self):
        return self._ymin
    def get_ymax(self):
        return self._ymax
    def get_zmin(self):
        return self._zmin
    def get_zmax(self):
        return self._zmax

    def set_xmin(self, value):
        self._xmin = value
    def set_xmax(self, value):
        self._xmax = value
    def set_ymin(self, value):
        self._ymin = value
    def set_ymax(self, value):
        self._ymax = value
    def set_zmin(self, value):
        self._zmin = value
    def set_zmax(self, value):
        self._zmax = value

    def del_xmin(self):
        del self._xmin
    def del_xmax(self):
        del self._xmax
    def del_ymin(self):
        del self._ymin
    def del_ymax(self):
        del self._ymax
    def del_zmin(self):
        del self._zmin
    def del_zmax(self):
        del self._zmax

    xmin = property(get_xmin, set_xmin, del_xmin, "xmin's docstring")
    xmax = property(get_xmax, set_xmax, del_xmax, "xmax's docstring")
    ymin = property(get_ymin, set_ymin, del_ymin, "ymin's docstring")
    ymax = property(get_ymax, set_ymax, del_ymax, "ymax's docstring")
    zmin = property(get_zmin, set_zmin, del_zmin, "zmin's docstring")
    zmax = property(get_zmax, set_zmax, del_zmax, "zmax's docstring")
        