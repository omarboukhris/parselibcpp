
from .observer import Observer
from string import Template

class GatewayGenerator(Observer):

	ns_template = "namespace $ns_name {\n\n"

	gateway = "\
extern \"C\" {\n\
\
$constructors\
$methods\
$accessors\
$destructor\
}\n"

	constructor_template = "\
$classname * _$classname_construct__($args) {\n\
\treturn new $classname($args);\n\
}\n\n"

	method_template = "\
$ret_type _$classname_$methname__($classname *self, args) {\n\
\t$content\n\
}\n\n"

	destructor_template = "\
void _delete_$classname__($classname *self) {\n\
\tdelete self;\n\
}\n\n"

	accessor_template = "\
void _$classname_set_$attrname__($classname *self, $attrtype $attrname) {\n\
\tself->set_$attrname ($attrname);\n\
}\n\n\
$attrtype _$classname_get_$attrname__($classname *self) {\n\
\treturn self->get_$attrname() ;\n\
}\n\n"

	ns_temp = Template(ns_template)
	gw_templ = Template(gateway)
	construct_templ = Template(constructor_template)
	method_templ = Template(method_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	def __init__(self, stream: callable):
		super(GatewayGenerator, self).__init__(stream)


	def process_namespace(self):
		ss = ""
		for ns in self.namespace:
			ss += GatewayGenerator.ns_temp.substitute(ns_name=ns)

		self.stream(ss)

	def end_process_namespace(self):
		ss = ""
		for ns in self.namespace:
			ss += "}\n\n"

		self.stream(ss)

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		# this is where processing goes
		pass

		
