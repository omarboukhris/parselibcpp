
from .observer import Observer

from string import Template

class HppGenerator(Observer):

	class_template = "\n\
class $classname {\n\
\n\
$public_methods\
$public_attributes\
$private_methods\
$private_attributes\
$protected_methods\
$protected_attributes\
} ;\n"

	attr_template = "\t$type $name;\n"

	meth_template = "\t$type $name ($args);\n"

	class_temp = Template(class_template)
	attr_temp = Template(attr_template)
	meth_temp = Template(meth_template)

	def __init__(self, stream: callable):
		super(HppGenerator, self).__init__(stream)

	def process_import(self, filenames=[]):
		import_list = ["#include " + fn for fn in filenames]
		ss = "\n".join(import_list)
		self.stream(ss)

	def process_class(self, t_class=[]):
		ss = ""
		for cl in t_class:
			ss += HppGenerator.class_temp.substitute(
				classname=cl.name,
				public_attributes=HppGenerator.process_attributes("public", cl.attributes),
				public_methods=HppGenerator.process_methods("public", cl.methods),
				private_attributes=HppGenerator.process_attributes("private", cl.attributes),
				private_methods=HppGenerator.process_methods("private", cl.methods),
				protected_attributes=HppGenerator.process_attributes("protected", cl.attributes),
				protected_methods=HppGenerator.process_methods("protected", cl.methods),
			)

		self.stream(ss)

	@staticmethod
	def process_attributes(visibility, attrs: list):
		ss = ""
		for attr in attrs:
			if attr.visibility == visibility:
				ss += HppGenerator.attr_temp.substitute(
					type=attr.type,
					name=attr.name
				)
		if ss != "":
			ss = "{visibility}:\n".format(visibility=visibility) + ss + "\n"
		return ss

	@staticmethod
	def process_methods(visibility, meths: list):
		ss = ""
		for meth in meths:
			if meth.visibility == visibility:
				ss += HppGenerator.meth_temp.substitute(
					type=meth.type,
					name=meth.name,
					args=HppGenerator.process_args(meth.args)
				)
		if ss != "":
			ss = "{visibility}:\n".format(visibility=visibility) + ss + "\n"
		return ss

	@staticmethod
	def process_args(args: list):
		ss = ""
		for arg in args:
			ss += "{type} {name}, ".format(type=arg.type, name=arg.name)
		return ss[:-2]

