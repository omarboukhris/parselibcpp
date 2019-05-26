#include <generator/generator.hpp>
#include <parselib/utils/io.hpp>

namespace generator {

TemplateGenerator::TemplateGenerator(std::string filename) {
	this->templateSourceCode = parselib::utils::gettextfilecontent(filename) ;
}

void TemplateGenerator::process(pt::ptree tree) {

}




}
