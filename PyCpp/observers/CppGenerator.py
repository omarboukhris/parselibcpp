
from .observer import Observer


class CppGenerator(Observer):

	def __init__(self, stream: callable):
		self.stream = stream

	def process_import(self, filenames=[]):
		import_list = ["#include " + fn for fn in filenames]
		ss = "\n".join(import_list)
		self.stream(ss)

	def process_class(self, t_class=[]):
		self.stream(t_class)
		pass

