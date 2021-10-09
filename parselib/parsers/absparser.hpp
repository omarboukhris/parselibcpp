#pragma once

//#include <parselib/parsers/lexer.hpp>

#include <parselib/datastructure/common_types.h>
#include <parselib/parsers/grammarparser.hpp>
#include <parselib/datastructure/parsetree.hpp>

namespace parselib {

typedef std::vector<parsetree::Node*> Frame ;
typedef std::vector<Frame> Row ;
typedef std::vector<Row> CYKMatrix ;

namespace myparsers {

class AbstractParser {
public :

	ProductionRules production_rules ;
	Grammar::UnitRelation unitrelation ;

	AbstractParser () {} 
	virtual ~AbstractParser () {} 
	AbstractParser (Grammar grammar) {
		production_rules = grammar.production_rules ;
		unitrelation = grammar.unitrelation ;
	}

	virtual Frame membership (TokenList word) = 0;

};

}

}
