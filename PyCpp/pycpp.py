
from PyCpp.observers.observer import *


class PyCppEngine:

	def __init__(self, json_obj, observers: list):
		self.json_obj = json_obj
		self.observers = observers

	def drive(self):
		# print (self.json_obj)
		for arr in self.json_obj["file"]:
			self.write_import(arr["imports"][0])
			self.write_class(arr["classdef"][0])

	def write_import(self, imports):
		import_list = []
		for files in imports["importfile"]:
			import_fname = "".join(files.split())
			import_list.append(import_fname)

		for obs in self.observers:
			obs.process_import(import_list)

	def write_class(self, classdef):
		out = []
		for classname, classbody in zip(classdef["classname"], classdef["classbody"]):
			classobj = Class(
				name=classname,
				doxy=classdef["doxy"] if "doxy" in classdef.keys() else "",
				constructs=PyCppEngine.write_construct(classbody["constructor"]),
				attributes=PyCppEngine.write_attr(classbody["attribute"]),
				methods=PyCppEngine.write_meth(classbody["method"])
			)
			out.append(classobj)

		for obs in self.observers:
			obs.process_class(out)

	@staticmethod
	def write_construct(construct_list):
		out = []
		for construct in construct_list:
			out.append(Construct(
				doxy=construct["doxy"][0] if "doxy" in construct.keys() else "",
				construct_type=construct["construct_type"][0],
				args=PyCppEngine.process_args(construct),
				core=construct["construct_core"][0] if "construct_core" in construct.keys() else ""
			))
		return out

	@staticmethod
	def write_attr(attr_list):
		out = []
		for attr in attr_list:
			out.append(Attribute(
				doxy=attr["doxy"][0] if "doxy" in attr.keys() else "",
				visibility=attr["visibility"][0],
				py="py" in attr.keys(),
				type=PyCppEngine.process_type(attr),
				name=attr["att_name"][0]
			))
		return out

	@staticmethod
	def write_meth(meth_list):
		out = []
		for meth in meth_list:
			out.append(Method(
				doxy=meth["doxy"][0] if "doxy" in meth.keys() else "",
				visibility=meth["visibility"][0],
				py="py" in meth.keys(),
				type=PyCppEngine.process_type(meth),
				name=meth["met_name"][0],
				args=PyCppEngine.process_args(meth),
				core=meth["meth_core"][0] if "meth_core" in meth.keys() else ""
			))
		return out

	@staticmethod
	def process_type(attr):
		attr_spl = attr["type"][0].split()
		if attr_spl[0] == "const":
			ty = "const " + "".join(attr_spl[1:])
		else:
			ty = "".join(attr_spl)
		ty = ty.replace(",", ", ")
		return ty

	@staticmethod
	def process_args(meth):
		out = []
		if "args" in meth.keys():
			_args = meth["args"][0]
			for argname, argtype in zip(_args["args_name"], _args["type"]):
				out.append(Args(type=argtype, name=argname))

		return out

