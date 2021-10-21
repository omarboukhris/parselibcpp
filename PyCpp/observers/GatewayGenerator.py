
from .observer import Observer
from string import Template

class GatewayGenerator(Observer):

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

	gw_templ = Template(gateway)
	construct_templ = Template(constructor_template)
	method_templ = Template(method_template)
	accessor_templ = Template(accessor_template)
	destructor_templ = Template(destructor_template)

	def __init__(self, stream: callable):
		super(GatewayGenerator, self).__init__(stream)

	def process_import(self, filenames=[]):
		pass

	def process_class(self, t_class=[]):
		# this is where processing goes
		pass

		
