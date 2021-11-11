
from observers.observer import *


class PyCppEngine:

	def __init__(self, json_obj, observers: list):
		self.json_obj = json_obj
		self.observers = observers

	def drive(self):
		""" Launches json data structure parsing into named tuples
		"""

		for arr in self.json_obj["file"]:
			self.register_module(arr["module_header"][0])
			self.write_import(arr["imports"][0])
			self.write_ns()
			self.write_class(arr["classdef"][0])
			self.end_write_ns()

	def register_module(self, mod_headers) -> None:
		""" Registers namespaces in observers for later use in code generation

		:param mod_headers: module headers
		:return: None
		"""
		# ns stands for namespace
		module_ns = [ns.strip() for ns in mod_headers["module_name"][0].split(".")]

		for obs in self.observers:
			obs.register_module(module_ns)

	def write_ns(self) -> None:
		""" Call namespace processing on all observers

		:return: None
		"""
		for obs in self.observers:
			obs.process_namespace()

	def end_write_ns(self) -> None:
		""" Finishes namespace processing on all observers

		:return: None
		"""
		for obs in self.observers:
			obs.end_process_namespace()

	def write_import(self, imports) -> None:
		""" Process imports and calls code generation on observers

		:param imports: list of import files
		:return: None
		"""
		import_list = []
		for files in imports["importfile"]:
			import_fname = "".join(files.split())
			import_list.append(import_fname)

		for obs in self.observers:
			obs.process_import(import_list)

	def write_class(self, classdef) -> None:
		""" Process parsed classes and calls code generation in observers

		:param classdef: classes definition in a json object
		:return: None
		"""
		out = []
		for classname, classbody in zip(classdef["classname"], classdef["classbody"]):
			classobj = Class(
				name=classname,
				doxy=classdef["doxy"] if "doxy" in classdef.keys() else "",
				inherit=classdef["inheritence"] if "inheritence" in classdef.keys() else "",
				constructs=PyCppEngine.write_construct(classbody["constructor"]) if "constructor" in classbody.keys() else [],
				attributes=PyCppEngine.write_attr(classbody["attribute"]) if "attribute" in classbody.keys() else [],
				methods=PyCppEngine.write_meth(classbody["method"]) if "method" in classbody.keys() else []
			)
			out.append(classobj)

		for obs in self.observers:
			obs.process_class(out)

	@staticmethod
	def write_construct(construct_list) -> list:
		""" Processes constructor json node

		:param construct_list: list of constructor json nodes
		:return: constructors processed in a list of named tuples
		"""
		out = []
		for construct in construct_list:
			out.append(Construct(
				doxy=construct["doxy"][0] if "doxy" in construct.keys() else "",
				construct_type=construct["construct_type"][0],
				args=PyCppEngine.process_args(construct),
				core=construct["construct_core"][0] if "construct_core" in construct.keys() else ""
			))
		destruct = Construct(doxy=None, construct_type="destructor", core="", args=[])
		out.append(destruct)
		return out

	@staticmethod
	def write_attr(attr_list) -> list:
		""" Processes attributes json node

		:param attr_list: list of attributes json nodes
		:return: attributes processed in a list of named tuples
		"""
		out = []
		for attr in attr_list:
			out.append(Attribute(
				doxy=attr["doxy"][0] if "doxy" in attr.keys() else "",
				# change this line if needed, default visibility on attribute is private
				visibility=attr["visibility"][0] if "visibility" in attr.keys() else "private",
				py="py" in attr.keys(),
				type=PyCppEngine.process_type(attr),
				name=attr["att_name"][0]
			))
		return out

	@staticmethod
	def write_meth(meth_list) -> list:
		""" Processes methods json node

		:param meth_list: list of methods json nodes
		:return: methods processed in a list of named tuples
		"""
		out = []
		for meth in meth_list:
			out.append(Method(
				doxy=meth["doxy"][0] if "doxy" in meth.keys() else "",
				# change this line if needed, default visibility on method is public
				visibility=meth["visibility"][0] if "visibility" in meth.keys() else "public",
				py="py" in meth.keys(),
				type=PyCppEngine.process_type(meth),
				name=meth["met_name"][0],
				args=PyCppEngine.process_args(meth),
				core=meth["meth_core"][0] if "meth_core" in meth.keys() else ""
			))
		return out

	@staticmethod
	def process_type(attr) -> str:
		""" processes type json node

		:param attr: attribute
		:return: attribute types processed in a list of named tuples
		"""
		attr_spl = attr["type"][0].split()
		if attr_spl[0] == "const":
			ty = "const " + "".join(attr_spl[1:])
		else:
			ty = "".join(attr_spl)
		ty = ty.replace(",", ", ")
		return ty

	@staticmethod
	def process_args(meth) -> list:
		""" Processes method arguments json node

		:param meth: method json node to process
		:return: arguments processed in a list of named tuples
		"""
		out = []
		if "args" in meth.keys():
			_args = meth["args"][0]
			for argname, argtype in zip(_args["args_name"], _args["type"]):
				out.append(Args(type=argtype, name=argname))

		return out

