

class OptimizationParameter():
    __name=""
    __min=0
    __max=-1
    __original=0
    __descetization=0
    __type="null"
    
    
    def __init__(self, name="", min_=1, max_=-1, original=0, descretization=0, type="null"):
        self.__descetization=descretization
        self.__min = min_
        self.__max = max_
        self.__name = name
        self.__original = original
        self.set_type(type)

    def get_type(self):
        return self.__type


    def set_type(self, value):
        self.__type = value


    def del_type(self):
        del self.__type


    def get_name(self):
        return self.__name


    def get_min(self):
        return self.__min


    def get_max(self):
        return self.__max


    def get_original(self):
        return self.__original


    def get_descetization(self):
        return self.__descetization


    def set_name(self, value):
        self.__name = value


    def set_min(self, value):
        self.__min = value


    def set_max(self, value):
        self.__max = value


    def set_original(self, value):
        self.__original = value


    def set_descetization(self, value):
        self.__descetization = value


    def del_name(self):
        del self.__name


    def del_min(self):
        del self.__min


    def del_max(self):
        del self.__max


    def del_original(self):
        del self.__original


    def del_descetization(self):
        del self.__descetization

    name = property(get_name, set_name, del_name, "name's docstring")
    min = property(get_min, set_min, del_min, "min's docstring")
    max = property(get_max, set_max, del_max, "max's docstring")
    original = property(get_original, set_original, del_original, "original's docstring")
    descetization = property(get_descetization, set_descetization, del_descetization, "descetization's docstring")
    type = property(get_type, set_type, del_type, "type's docstring")
        
        
    