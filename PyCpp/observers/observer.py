
from collections import namedtuple
from string import Template

Args = namedtuple("Args", ["type", "name"])
Attribute = namedtuple("Attribute", ["doxy", "visibility", "py", "type", "name"])
Method = namedtuple("Method", ["doxy", "visibility", "py", "type", "name", "args", "core"])
Construct = namedtuple("Construct", ["doxy", "construct_type", "args", "core"])
Class = namedtuple("Class", ["doxy", "name", "inherit", "constructs", "attributes", "methods"])

class Observer:
	"""
	This is the main Abstract class describing a generator for the PyCpp language
	The engine uses all functions and sub classes should implement the methods approriately
	"""

	def __init__(self, stream: callable):
		self.stream = stream
		self.namespace = []

	def process_namespace(self):
		pass

	def end_process_namespace(self):
		pass

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		pass

	def register_module(self, t_module=[]):
		self.namespace = t_module


class CppAbstractObs(Observer):
	"""
	Abstract factorized C++ Helper generator
	Methods below are common to C++ generators
	"""

	ns_template = "namespace $ns_name {\n\n"
	ns_temp = Template(ns_template)

	attr_template = "$doc\n\t$type $name;\n\n"
	attr_temp = Template(attr_template)

	def __init__(self, stream: callable):
		super(CppAbstractObs, self).__init__(stream)

	def process_import(self, filenames: list = None):
		if not filenames:
			filenames = []
		import_list = ["#include " + fn for fn in filenames]
		ss = "\n".join(import_list) + "\n\n"
		self.stream(ss)

	def process_namespace(self):
		ss = ""
		for ns in self.namespace:
			ss += CppAbstractObs.ns_temp.substitute(ns_name=ns)

		self.stream(ss)

	def end_process_namespace(self):
		ss = ""
		for ns in self.namespace:
			ss += "}} // namespace {}\n\n".format(ns)

		self.stream(ss)

	@classmethod
	def process_core(cls, strcore: str, level: int = 1, tabsz: int = 2):
		""" strips the @{ @} tokens from a function core """
		if not strcore:
			return ""
		elif strcore.find("{{") == 0 and strcore.find("}}") == len(strcore)-2:
			strcore = strcore[2:-2]
			split_core = strcore.split("\n")

			assert (level > 0)
			tab = "\t" * level
			delim = "\n" + tab
			# merge_core = tab + delim.join([sc.strip() for sc in split_core])
			merge_core = tab + delim.join([sc[tabsz:] for sc in split_core])
			return merge_core[:-1]
		else:
			# ill formed core somehow
			raise Exception("Ill formed function core <{}>".format(strcore))

	@classmethod
	def process_args(cls, args: list):
		ss = ""
		for arg in args:
			ss += "{type} {name}, ".format(type=arg.type, name=arg.name)
		return ss[:-2]

	@classmethod
	def process_doc(cls, doc: str, level: int = 1):
		if not doc:
			doc = "// No documentation specified"
		doc_split = doc.split("\n")
		if len(doc_split) == 1:
			doc = "\t" * level + doc.strip()
		else:
			tab = "\t" * level
			delim = "\n" + tab
			doc = tab + delim.join([d.strip() for d in doc_split])
		return doc

	@classmethod
	def process_attributes(cls, visibility, attrs: list):
		ss = ""
		for attr in attrs:
			if attr.visibility == visibility:
				ss += CppAbstractObs.attr_temp.substitute(
					doc=cls.process_doc(attr.doxy),
					type=attr.type,
					name=attr.name
				)
		if ss != "":
			ss = "{visibility}:\n".format(visibility=visibility) + ss + "\n"
		return ss
