
from collections import namedtuple


Args = namedtuple("Args", ["type", "name"])
Attribute = namedtuple("Attribute", ["visibility", "type", "name"])
Method = namedtuple("Method", ["visibility", "type", "name", "args"])
Class = namedtuple("Class", ["name", "attributes", "methods"])

class Observer:

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		pass
