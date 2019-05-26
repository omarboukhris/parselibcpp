#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree ;

namespace generator {

// namespace pt = boost::property_tree ;
// should be a template engine
class TemplateGenerator {
public :
	TemplateGenerator (std::string filename) ;
	void process (pt::ptree tree) ;
protected :
	std::string templateSourceCode ;
} ;

}
