
from collections import namedtuple

Args = namedtuple("Args", ["type", "name"])
Attribute = namedtuple("Attribute", ["visibility", "py", "type", "name"])
Method = namedtuple("Method", ["visibility", "py", "type", "name", "args"])
Class = namedtuple("Class", ["name", "attributes", "methods"])

class Observer:

	def __init__(self, stream: callable):
		self.stream = stream

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		pass
