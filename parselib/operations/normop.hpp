#pragma once
#include "naiveparsers.hpp"

#include <parselib/datastructure/lexer.hpp>
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/parsers/grammarparser.hpp>

namespace parselib {

namespace normoperators {

myparsers::Grammar get2nf (myparsers::Grammar grammar) ;

myparsers::Grammar removenullables (myparsers::Grammar grammar) ;

StrList getnullables (myparsers::Grammar grammar) ;

myparsers::Grammar getunitrelation (myparsers::Grammar grammar) ;

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
