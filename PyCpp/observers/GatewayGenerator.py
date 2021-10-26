
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
${classname} * _${classname}_construct__(${args}) {\n\
\treturn new ${classname}(${args});\n\
}\n\n"

	method_template = "\
${type} _${classname}_${methname}__(${classname} *self, ${args}) \
{ ${content} }\n\n"

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
	method_templ = Template(method_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	def __init__(self, stream: callable):
		super(GatewayGenerator, self).__init__(stream)

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
		for construct in constructors:
			if construct.construct_type == "constructor":
				ss += GatewayGenerator.construct_templ.substitute(
					classname=classname,
					args=cls.process_args(construct.args),
					content=cls.process_core(construct.core, level=1)
				)
		return ss

	@classmethod
	def process_methods(cls, meths: list, clname: str):
		ss = ""
		for meth in meths:
			if meth.visibility == "public":
				ss += GatewayGenerator.method_templ.substitute(
					type=meth.type,
					classname=clname,
					methname=meth.name,
					args=cls.process_args(meth.args),
					content=cls.process_core(meth.core, level=1)
				)
		return ss

	@classmethod
	def process_accessors(cls, attrs: list, clname: str):
		ss = ""
		for attr in attrs:
			if attr.visibility == "public":
				ss += GatewayGenerator.accessor_templ.substitute(
					type=attr.type,
					classname=clname,
					attrname=attr.name
				)
		return ss

	@classmethod
	def process_destructor(cls, clname: str):
		return GatewayGenerator.destructor_templ.substitute(classname=clname)
