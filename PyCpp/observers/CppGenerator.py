
from .observer import Observer

from string import Template

class CppGenerator(Observer):

	constructor_template = "$classname::$classname ($args) {$content}\n\n"
	meth_template = "$type $classname::$name ($args) {$content}\n\n"

	meth_temp = Template(meth_template)
	constructor_temp = Template(constructor_template)

	def __init__(self, stream: callable):
		super(CppGenerator, self).__init__(stream)

	def process_import(self, filenames=[]):
		import_list = ["#include " + fn for fn in filenames]
		ss = "\n".join(import_list) + "\n\n"
		self.stream(ss)

	def process_class(self, t_class=[]):
		ss = ""
		for cl in t_class:
			for construct in cl.constructs:
				ss += CppGenerator.constructor_temp.substitute(
					classname=cl.name,
					args=CppGenerator.process_args(construct.args),
					content=CppGenerator.process_core(construct.core)
				)

			for method in cl.methods:
				ss += CppGenerator.meth_temp.substitute(
					type=method.type,
					classname=cl.name,
					name=method.name,
					args=CppGenerator.process_args(method.args),
					content=CppGenerator.process_core(method.core)
				)

		self.stream(ss)

	@staticmethod
	def process_args(args: list):
		ss = ""
		for arg in args:
			ss += "{type} {name}, ".format(type=arg.type, name=arg.name)
		return ss[:-2]

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
