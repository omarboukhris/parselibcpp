
from .observer import CppAbstractObs

from string import Template

class CppGenerator(CppAbstractObs):

	constructor_template = "$classname::${constr_type} ($args) {$content}\n\n"
	meth_template = "$type $classname::$name ($args) {$content}\n\n"

	meth_temp = Template(meth_template)
	constructor_temp = Template(constructor_template)

	def __init__(self, stream: callable):
		super(CppGenerator, self).__init__(stream)

	def process_class(self, t_class=None):
		# solves immutable object as default param
		if not t_class:
			t_class = []

		ss = ""
		for cl in t_class:
			for construct in cl.constructs:
				cname_prefix = "" if construct.construct_type == "constructor" else "~"
				ss += CppGenerator.constructor_temp.substitute(
					classname=cl.name.strip(),
					constr_type=cname_prefix + cl.name.strip(),
					args=self.process_args(construct.args),
					content=self.process_core(construct.core, level=1)
				)

			for method in cl.methods:
				ss += CppGenerator.meth_temp.substitute(
					type=method.type,
					classname=cl.name.strip(),
					name=method.name,
					args=self.process_args(method.args),
					content=self.process_core(method.core, level=1)
				)

		self.stream(ss)

