
from .GatewayGenerator import GatewayGenerator

from string import Template

class PyGwGenerator(GatewayGenerator):

	import_template = "\nimport ctypes\n"

	gateway = "\n\
class $classname:\n\n\
\t${modulename} = ctypes.cdll.LoadLibrary(\"change/path\")\n\
${special_type_def}\n\n\
\tdef __init__(self):\n\
\t\tself.this_ = None\n\n\
${constructors}\
${methods}\
${accessors}\
${destructor}\n\n\
if __name__ == \"__main__\":\n\
\t# magic happens here\n\tpass\n"

	constructor_template = "\
\tdef construct_object_$num(self, ${args}):\n\
\t\tself.this_ = ${classname}.${modulename}._${classname}_construct_${num}__($args)\n\n"

	method_fn_template = "\
\tdef ${methname}(self${args}):\n\
\t\treturn ${classname}.${modulename}._${classname}_${methname}__(self.this_, ${args})\n\n"
	method_proc_template = "\
\tdef ${methname}(self${args}):\n\
\t\t${classname}.${modulename}._${classname}_${methname}__(self.this_, ${args})${extend_fn}\n\n"

	destructor_template = "\
\tdef __del__(self):\n\
\t\t${classname}.${modulename}._delete_${classname}__(self.this_)\n\n"

	accessor_template = "\
\tdef set_${attrname}(self, ${attrname}):\n\
\t\t${classname}.${modulename}._${classname}_set_${attrname}__ (self.this_, ${attrname})\n\n\
\tdef get_${attrname}(self):\n\
\t\treturn ${classname}.${modulename}._${classname}_get_${attrname}__(self.this_)\n\n"

	import_templ = Template(import_template)
	gw_templ = Template(gateway)
	construct_templ = Template(constructor_template)
	method_fn_templ = Template(method_fn_template)
	method_proc_templ = Template(method_proc_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	string_type_def_templ = Template(
		"${classname}.${modulename}._${classname}_${methname}__.res_type = ctypes.c_char_p\n")

	def __init__(self, stream: callable):
		super(PyGwGenerator, self).__init__(stream)

	def process_class(self, t_class=[]):
		# this is where processing goes
		ss = ""
		ss += PyGwGenerator.import_templ.substitute()
		for cl in t_class:
			clname = cl.name.strip()
			modulename = clname.capitalize()
			ss += PyGwGenerator.gw_templ.substitute(
				classname=clname,
				modulename=modulename,
				special_type_def=self.process_special_type_def(cl.methods, clname),
				constructors=self.process_constructors(cl.constructs, clname),
				methods=self.process_methods(cl.methods, clname),
				accessors=self.process_accessors(cl.attributes, clname),
				destructor=self.process_destructor(clname)
			)
		self.stream(ss)

	@classmethod
	def process_special_type_def(cls, methods: list, clname: str):
		ss = ""
		for meth in methods:
			if meth.visibility == "public" and meth.type == "string":
				ss += "\t{}".format(PyGwGenerator.string_type_def_templ.substitute(
					type=meth.type,
					classname=clname,
					modulename=clname.capitalize(),
					methname=meth.name,
				))
		return ss

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		num = 0
		modulename = classname.capitalize()
		for construct in constructors:
			if construct.construct_type == "constructor":
				args = cls.process_t_args(construct.args)
				# str_args = ", {}".format(args) if args.strip() else ""
				ss += PyGwGenerator.construct_templ.substitute(
					classname=classname,
					num=num,
					args=args,
					content=cls.process_core(construct.core, level=1),
					modulename=modulename
				)
				num += 1
		return ss

	@classmethod
	def process_methods(cls, meths: list, clname: str):
		ss = ""
		for meth in meths:
			if meth.visibility == "public":
				# prepare template arguments
				args = cls.process_t_args(meth.args)
				str_args = ", {}".format(args) if args.strip() else ""
				templ = PyGwGenerator.method_fn_templ
				extended = ".decode()" if meth.type == "string" else ""
				if meth.type == "void":
					templ = PyGwGenerator.method_proc_templ

				# substitute in template
				ss += templ.substitute(
					type=meth.type,
					classname=clname,
					modulename=clname.capitalize(),
					methname=meth.name,
					args=str_args,
					content=cls.process_core(meth.core, level=1),
					extend_fn=extended
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
					modulename=clname.capitalize(),
					attrname=attr.name
				)
		return ss

	@classmethod
	def process_destructor(cls, clname: str):
		return PyGwGenerator.destructor_templ.substitute(classname=clname, modulename=clname.capitalize())

	def process_import(self, filenames=[]):
		pass

	def process_namespace(self):
		pass

	def end_process_namespace(self):
		pass
