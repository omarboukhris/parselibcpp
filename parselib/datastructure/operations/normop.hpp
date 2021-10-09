#pragma once

#include <parselib/datastructure/common_types.h>
#include <parselib/parsers/grammarparser.hpp>

namespace parselib {

namespace normoperators {

Grammar get2nf (Grammar grammar) ;

Grammar removenullables (Grammar grammar) ;

StrList getnullables (Grammar grammar) ;

Grammar getunitrelation (Grammar grammar) ;

class TERM {
public :
	ProductionRules production_rules ;
	TERM (ProductionRules production_rules) ;		
	void apply () ;

private :
	ProductionRules normalForm ;
	
	void term () ;
	void checkruleforterminals (std::string key, Rule rule) ;
} ;

class BIN {
public :
	ProductionRules production_rules ;
	BIN (ProductionRules production_rules) ;
	void apply () ;

private :	
	ProductionRules normalForm ;

	void binarize () ;
	bool binonce () ;

	void binarizerule (std::string key, Rule rule) ;
} ;


} //namespace normoperators

} //namespace parselib
