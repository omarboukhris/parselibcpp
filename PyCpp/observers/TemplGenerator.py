
from .observer import CppAbstractObs

from string import Template

class TemplGenerator(CppAbstractObs):

	ns_template = "namespace $ns_name {\n\n"

	class_template = "\n\
$doc\n\
class $classname\
$inheritence {\n\
\n\
$constructors\
$public_methods\
$public_attributes\
$private_methods\
$private_attributes\
$protected_methods\
$protected_attributes\
} ;\n\n"

	construct_template = "$doc\n\t$classname ($args) {$construct_core}\n\n"

	meth_template = "$doc\n\t$type $name ($args) {$meth_core}\n\n"

	ns_temp = Template(ns_template)
	class_temp = Template(class_template)
	const_temp = Template(construct_template)
	meth_temp = Template(meth_template)

	def __init__(self, stream: callable):
		super(TemplGenerator, self).__init__(stream)

	def process_class(self, t_class=[]):
		ss = ""
		for cl in t_class:
			ss += TemplGenerator.class_temp.substitute(
				doc=cl.doxy if cl.doxy else "// No class documentation was specified",
				classname=cl.name,
				inheritence=": {}".format(cl.inherit[0]) if cl.inherit else "",
				constructors=TemplGenerator.process_constructors(cl.constructs, cl.name),
				public_attributes=self.process_attributes("public", cl.attributes),
				public_methods=TemplGenerator.process_methods("public", cl.methods),
				private_attributes=self.process_attributes("private", cl.attributes),
				private_methods=TemplGenerator.process_methods("private", cl.methods),
				protected_attributes=self.process_attributes("protected", cl.attributes),
				protected_methods=TemplGenerator.process_methods("protected", cl.methods),
			)

		self.stream(ss)

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		for construct in constructors:
			cname_prefix = "" if construct.construct_type == "constructor" else "~"
			ss += TemplGenerator.const_temp.substitute(
				doc=cls.process_doc(construct.doxy),
				classname=cname_prefix + classname,
				args=cls.process_args(construct.args),
				construct_core=cls.process_core(construct.core, level=2)
			)
		if ss != "":
			ss = "public:\n" + ss
		return ss

	@classmethod
	def process_methods(cls, visibility, meths: list):
		ss = ""
		for meth in meths:
			if meth.visibility == visibility:
				ss += TemplGenerator.meth_temp.substitute(
					doc=TemplGenerator.process_doc(meth.doxy),
					type=meth.type,
					name=meth.name,
					args=cls.process_args(meth.args),
					meth_core=cls.process_core(meth.core, level=2)
				)
		if ss != "":
			ss = "{visibility}:\n".format(visibility=visibility) + ss + "\n"
		return ss

