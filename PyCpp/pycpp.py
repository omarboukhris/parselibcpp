
from .observers.observer import *


class PyCpp:

	def __init__(self, json_obj, observers=[]):
		self.json_obj = json_obj
		self.observers = observers

	def make_hpp(self):
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
				attributes=PyCpp.write_attr(classbody["attribute"]),
				methods=PyCpp.write_meth(classbody["method"])
			)
			out.append(classobj)

		for obs in self.observers:
			obs.process_class(out)

	@staticmethod
	def write_attr(attr_list):
		out = []
		for attr in attr_list:
			out.append(Attribute(
				visibility=attr["visibility"][0],
				type=PyCpp.process_type(attr),
				name=attr["att_name"][0]
			))
		return out

	@staticmethod
	def write_meth(meth_list):
		out = []
		for meth in meth_list:
			out.append(Method(
				visibility=meth["visibility"][0],
				type=PyCpp.process_type(meth),
				name=meth["met_name"][0],
				args=PyCpp.process_args(meth)
			))
		return out

	@staticmethod
	def process_type(attr):
		ty = "".join(attr["type"][0].split())
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

