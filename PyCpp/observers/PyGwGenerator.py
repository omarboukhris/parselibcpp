
from .GatewayGenerator import GatewayGenerator

from string import Template

class PyGwGenerator(GatewayGenerator):

	import_template = "\nimport ctype.$module # change accordingly\n"

	gateway = "\n\
class $classname:\n\n\
${constructors}\
${methods}\
${accessors}\
${destructor}\n\n"

	constructor_template = "\
\tdef __init__(self${args}) {\n\
\t\tself.this_ = $classname($args)\n\n"

	method_template = "\
\tdef ${methname}(self${args}) :\n\
\t\t${classname}.${modulename}._${classname}_${methname}__(${args})\n\n"

	destructor_template = "\
\tdef __del__(self) {\n\
\t\t${classname}.${modulename}._delete_${classname}__(self.this_)\n\n"

	accessor_template = "\
\tdef set_${attrname}(self, ${attrname}):\n\
\t\t${classname}.${modulename}._${classname}_set_${attrname}__ (self.this_, ${attrname})\n\n\
\tdef  get_${attrname}(self):\n\
\t\treturn _${classname}_get_${attrname}__(self.this_)\n\n"

	import_templ = Template(import_template)
	gw_templ = Template(gateway)
	construct_templ = Template(constructor_template)
	method_templ = Template(method_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	def __init__(self, stream: callable):
		super(PyGwGenerator, self).__init__(stream)

	def process_import(self, filenames=[]):
		pass

	def process_namespace(self):
		pass

	def end_process_namespace(self):
		pass

	def process_class(self, t_class=[]):
		# this is where processing goes
		ss = ""
		for cl in t_class:
			clname = cl.name.strip()
			ss += PyGwGenerator.import_templ.substitute(
				module=clname
			)

			ss += PyGwGenerator.gw_templ.substitute(
				classname=clname,
				constructors=self.process_constructors(cl.constructs, clname),
				methods=self.process_methods(cl.methods, clname),
				accessors=self.process_accessors(cl.attributes, clname),
				destructor=self.process_destructor(clname)
			)
		self.stream(ss)

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		for construct in constructors:
			if construct.construct_type == "constructor":
				args = cls.process_args(construct.args)
				str_args = ", {}".format(args) if args.strip() else ""
				ss += PyGwGenerator.construct_templ.substitute(
					classname=classname,
					args=str_args,
					content=cls.process_core(construct.core, level=1)
				)
		return ss

	@classmethod
	def process_methods(cls, meths: list, clname: str):
		ss = ""
		for meth in meths:
			if meth.visibility == "public":
				args = cls.process_args(meth.args)
				str_args = ", {}".format(args) if args.strip() else ""
				ss += PyGwGenerator.method_templ.substitute(
					type=meth.type,
					classname=clname,
					modulename=clname,
					methname=meth.name,
					args=str_args,
					content=cls.process_core(meth.core, level=1)
				)
		return ss

	@classmethod
	def process_accessors(cls, attrs: list, clname: str):
		ss = ""
		for attr in attrs:
			if attr.visibility == "public":
				ss += PyGwGenerator.accessor_templ.substitute(
					type=attr.type,
					classname=clname,
					modulename=clname,
					attrname=attr.name
				)
		return ss

	@classmethod
	def process_destructor(cls, clname: str):
		return PyGwGenerator.destructor_templ.substitute(classname=clname, modulename=clname)

	@classmethod
	def process_args(cls, args: list):
		ss = ""
		for arg in args:
			ss += "{name}, ".format(name=arg.name)
		return ss[:-2]
