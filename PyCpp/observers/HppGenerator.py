
from .TemplGenerator import TemplGenerator

from string import Template

class HppGenerator(TemplGenerator):

	construct_template = "$doc\n\t$classname($args);\n\n"

	meth_template = "$doc\n\t$type $name ($args);\n\n"

	const_temp = Template(construct_template)
	meth_temp = Template(meth_template)

	def __init__(self, stream: callable):
		super(HppGenerator, self).__init__(stream)

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		for construct in constructors:
			cname_prefix = "" if construct.construct_type == "constructor" else "~"
			ss += HppGenerator.const_temp.substitute(
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
