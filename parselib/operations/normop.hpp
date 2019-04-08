#pragma once
#include "naiveparsers.hpp"

#include <parselib/datastructure/lexer.hpp>
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/parsers/grammarparser.hpp>

namespace parselib {

namespace normoperators {

myparsers::Grammar get2nf (myparsers::Grammar grammar) ;

myparsers::Grammar removenullables (myparsers::Grammar grammar) ;

SequentialParser::StrList getnullables (myparsers::Grammar grammar) ;

myparsers::Grammar getunitrelation (myparsers::Grammar grammar) ;

class TERM {
public :
	operations::SequentialParser::ProductionRules production_rules ;
	TERM (operations::SequentialParser::ProductionRules production_rules) ;		
	void apply () ;

private :
	operations::SequentialParser::ProductionRules normalForm ;
	
	void term () ;
	void checkruleforterminals (std::string key, operations::SequentialParser::Rule rule) ;
} ;

class BIN {
public :
	operations::SequentialParser::ProductionRules production_rules ;
	BIN (operations::SequentialParser::ProductionRules production_rules) ;
	void apply () ;

private :	
	operations::SequentialParser::ProductionRules normalForm ;

	void binarize () ;
	bool binonce () ;

	void binarizerule (std::string key, operations::SequentialParser::Rule rule) ;
} ;


} //namespace normoperators

} //namespace parselib
