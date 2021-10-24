
from .observer import Observer

from string import Template

class TemplGenerator(Observer):

	class_template = "\n\
$doc\n\
class $classname {\n\
\n\
$constructors\
$public_methods\
$public_attributes\
$private_methods\
$private_attributes\
$protected_methods\
$protected_attributes\
} ;\n"

	construct_template = "$doc\n\t$classname ($args) {$construct_core}\n\n"

	attr_template = "$doc\n\t$type $name;\n\n"

	meth_template = "$doc\n\t$type $name ($args) {$meth_core}\n\n"

	class_temp = Template(class_template)
	const_temp = Template(construct_template)
	attr_temp = Template(attr_template)
	meth_temp = Template(meth_template)

	def __init__(self, stream: callable):
		super(TemplGenerator, self).__init__(stream)

	def process_import(self, filenames=[]):
		import_list = ["#include " + fn for fn in filenames]
		ss = "\n".join(import_list)
		self.stream(ss)

	def process_class(self, t_class=[]):
		ss = ""
		for cl in t_class:
			ss += TemplGenerator.class_temp.substitute(
				doc=cl.doxy if cl.doxy else "// No class documentation was specified",
				classname=cl.name,
				constructors=TemplGenerator.process_constructors(cl.constructs, cl.name),
				public_attributes=TemplGenerator.process_attributes("public", cl.attributes),
				public_methods=TemplGenerator.process_methods("public", cl.methods),
				private_attributes=TemplGenerator.process_attributes("private", cl.attributes),
				private_methods=TemplGenerator.process_methods("private", cl.methods),
				protected_attributes=TemplGenerator.process_attributes("protected", cl.attributes),
				protected_methods=TemplGenerator.process_methods("protected", cl.methods),
			)

		self.stream(ss)

	@staticmethod
	def process_constructors(constructors: list, classname: str):
		ss = ""
		for construct in constructors:
			cname_prefix = "" if construct.construct_type == "constructor" else "~"
			ss += TemplGenerator.const_temp.substitute(
				doc=TemplGenerator.process_doc(construct.doxy),
				classname=cname_prefix + classname,
				args=TemplGenerator.process_args(construct.args),
				construct_core=TemplGenerator.process_core(construct.core)
			)
		if ss != "":
			ss = "public:\n" + ss
		return ss

	@staticmethod
	def process_attributes(visibility, attrs: list):
		ss = ""
		for attr in attrs:
			if attr.visibility == visibility:
				ss += TemplGenerator.attr_temp.substitute(
					doc=TemplGenerator.process_doc(attr.doxy),
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
				ss += TemplGenerator.meth_temp.substitute(
					doc=TemplGenerator.process_doc(meth.doxy),
					type=meth.type,
					name=meth.name,
					args=TemplGenerator.process_args(meth.args),
					meth_core=TemplGenerator.process_core(meth.core)
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

	@staticmethod
	def process_doc(doc: str):
		if not doc:
			doc = "// No documentation specified"
		doc_split = doc.split("\n")
		if len(doc_split) == 1:
			doc = "\t" + doc.strip()
		else:
			doc = "\t" + "\n\t".join([d.strip() for d in doc_split])
		return doc

	@staticmethod
	def process_core(strcore: str):
		""" strips the @{ @} tokens from a function core """
		if not strcore:
			return ""
		elif strcore.find("@{") == 0 and strcore.find("@}") == len(strcore)-2:
			strcore = strcore[2:-2]
			split_core = strcore.split("\n")
			merge_core = "\t\t" + "\n\t\t".join([sc.strip() for sc in split_core])
			return merge_core[:-1]
		else:
			# ill formed core somehow
			raise Exception("Ill formed function core <{}>".format(strcore))
