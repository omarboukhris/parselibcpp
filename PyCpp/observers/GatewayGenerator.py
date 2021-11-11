
from .observer import CppAbstractObs

from string import Template

class GatewayGenerator(CppAbstractObs):

	ns_template = "namespace $ns_name {\n\n"

	gateway = "\
extern \"C\" {\n\
\n\
${constructors}\
${methods}\
${accessors}\
${destructor}\n\
}\n\n"

	constructor_template = "\
${classname} * _${classname}_construct_${num}__(${args}) {\n\
\treturn new ${classname}(${t_args});\n\
}\n\n"

	method_fn_template = "\
${type} _${classname}_${methname}__(${classname} *self${args}) {\n\
\treturn self->${methname}(${t_args});\n\
}\n\n"
	method_proc_template = "\
void _${classname}_${methname}__(${classname} *self${args}) {\n\
\tself->${methname}(${t_args});\n\
}\n\n"
	method_str_template = "\
const char * _${classname}_${methname}__(${classname} *self${args}) {\n\
\tstatic std::string s_out;\n\
\ts_out = self->${methname}(${t_args});\n\
\treturn s_out.c_str();\n\
}\n\n"

	destructor_template = "\
void _delete_${classname}__(${classname} *self) {\n\
\tdelete self;\n\
}\n\n"

	accessor_template = "\
void _${classname}_set_${attrname}__(${classname} *self, ${type} ${attrname}) {\n\
\tself->set_${attrname} (${attrname});\n\
}\n\n\
${type} _${classname}_get_${attrname}__(${classname} *self) {\n\
\treturn self->get_${attrname}() ;\n\
}\n\n"

	ns_temp = Template(ns_template)
	gw_templ = Template(gateway)
	construct_templ = Template(constructor_template)
	method_fn_templ = Template(method_fn_template)
	method_proc_templ = Template(method_proc_template)
	method_str_templ = Template(method_str_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	def __init__(self, stream: callable):
		super(GatewayGenerator, self).__init__(stream)
		self.header_filename = ""

	def set_header_filename(self, filename):
		processed_fn = filename.split("/")[-1].split(".")[0] + ".h"
		self.header_filename = "\"{}\"".format(processed_fn)

	def process_import(self, filenames=[]):
		import_list = ["#include " + fn for fn in filenames + [self.header_filename]]
		ss = "\n".join(import_list) + "\n\n"
		self.stream(ss)

	def process_class(self, t_class=[]):
		# this is where processing goes
		ss = ""
		for cl in t_class:
			clname = cl.name.strip()
			ss += GatewayGenerator.gw_templ.substitute(
				constructors=self.process_constructors(cl.constructs, clname),
				methods=self.process_methods(cl.methods, clname),
				accessors=self.process_accessors(cl.attributes, clname),
				destructor=self.process_destructor(clname)
			)
		self.stream(ss)

	@classmethod
	def process_constructors(cls, constructors: list, classname: str):
		ss = ""
		num = 0
		for construct in constructors:
			if construct.construct_type == "constructor":
				ss += GatewayGenerator.construct_templ.substitute(
					classname=classname,
					args=cls.process_args(construct.args),
					num=num,
					content=cls.process_core(construct.core, level=1),
					t_args=cls.process_t_args(construct.args)
				)
				num += 1
		return ss

	@classmethod
	def process_methods(cls, meths: list, clname: str):
		ss = ""
		for meth in meths:
			if meth.visibility == "public":
				args = cls.process_args(meth.args)
				str_args = ", {}".format(args) if args.strip() else ""
				templ = GatewayGenerator.method_fn_templ
				if meth.type == "void":
					templ = GatewayGenerator.method_proc_templ
				elif meth.type in ["std::string", "string"]:
					templ = GatewayGenerator.method_str_templ
				ss += templ.substitute(
					type=meth.type,
					classname=clname,
					methname=meth.name,
					args=str_args,
					t_args=GatewayGenerator.process_t_args(meth.args),
				)
		return ss

	@classmethod
	def process_accessors(cls, attrs: list, clname: str):
		ss = ""
		for attr in attrs:
			if attr.visibility != "public":
				ss += GatewayGenerator.accessor_templ.substitute(
					type=attr.type,
					classname=clname,
					attrname=attr.name
				)
		return ss

	@classmethod
	def process_destructor(cls, clname: str):
		return GatewayGenerator.destructor_templ.substitute(classname=clname)

	@classmethod
	def process_t_args(cls, args: list):
		ss = ""
		for arg in args:
			ss += "{name}, ".format(name=arg.name)
		return ss[:-2]