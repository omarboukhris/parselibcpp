
from collections import namedtuple

Args = namedtuple("Args", ["type", "name"])
Attribute = namedtuple("Attribute", ["doxy", "visibility", "py", "type", "name"])
Method = namedtuple("Method", ["doxy", "visibility", "py", "type", "name", "args", "core"])
Construct = namedtuple("Construct", ["doxy", "construct_type", "args", "core"])
Class = namedtuple("Class", ["doxy", "name", "inherit", "constructs", "attributes", "methods"])

class Observer:

	def __init__(self, stream: callable):
		self.stream = stream

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		pass
