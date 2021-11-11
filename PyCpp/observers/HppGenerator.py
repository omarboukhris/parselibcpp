
from .CppGenerator import CppAbstractObs

from string import Template

class HppGenerator(CppAbstractObs):

	ns_template = "namespace $ns_name {\n\n"

	class_template = "\n\
$doc\n\
class $classname\
$inheritence {\n\
\n\
$constructors\
$public_methods\
$accessors\
$public_attributes\
$private_methods\
$private_attributes\
$protected_methods\
$protected_attributes\
} ;\n\n"

	accessor_template = "\
\tvoid set_${attrname} (${type} t_${attrname});\n\
\t${type} get_${attrname}() ;\n\n"

	construct_template = "$doc\n\t$classname($args);\n\n"

	meth_template = "$doc\n\t$type $name ($args);\n\n"

	ns_temp = Template(ns_template)
	class_temp = Template(class_template)

	construct_temp = Template(construct_template)
	meth_temp = Template(meth_template)

	accessor_templ = Template(accessor_template)

	def __init__(self, stream: callable):
		super(HppGenerator, self).__init__(stream)

	def process_class(self, t_class=[]):
		ss = ""
		for cl in t_class:
			ss += HppGenerator.class_temp.substitute(
				doc=cl.doxy if cl.doxy else "// No class documentation was specified",
				classname=cl.name,
				inheritence=": {}".format(cl.inherit[0]) if cl.inherit else "",
				constructors=HppGenerator.process_constructors(cl.constructs, cl.name),
				public_attributes=self.process_attributes("public", cl.attributes),
				public_methods=HppGenerator.process_methods("public", cl.methods),
				accessors=self.process_accessors(cl.attributes, cl.name),
				private_attributes=self.process_attributes("private", cl.attributes),
				private_methods=HppGenerator.process_methods("private", cl.methods),
				protected_attributes=self.process_attributes("protected", cl.attributes),
				protected_methods=HppGenerator.process_methods("protected", cl.methods),
			)

		self.stream(ss)

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		for construct in constructors:
			cname_prefix = "" if construct.construct_type == "constructor" else "~"
			ss += HppGenerator.construct_temp.substitute(
				doc=HppGenerator.process_doc(construct.doxy),
				classname=cname_prefix + classname,
				args=HppGenerator.process_args(construct.args)
			)
		if ss != "":
			ss = "public:\n" + ss
		return ss

	@classmethod
	def process_methods(cls, visibility, meths: list):
		ss = ""
		for meth in meths:
			if meth.visibility == visibility:
				ss += HppGenerator.meth_temp.substitute(
					doc=HppGenerator.process_doc(meth.doxy),
					type=meth.type,
					name=meth.name,
					args=HppGenerator.process_args(meth.args)
				)
		if ss != "":
			ss = "{visibility}:\n".format(visibility=visibility) + ss + "\n"
		return ss

	@classmethod
	def process_accessors(cls, attrs: list, clname: str):
		ss = ""
		for attr in attrs:
			if attr.visibility != "public":
				ss += HppGenerator.accessor_templ.substitute(
					type=attr.type,
					classname=clname,
					attrname=attr.name
				)
		return ss

