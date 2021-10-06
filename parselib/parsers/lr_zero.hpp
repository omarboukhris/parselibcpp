#pragma once

#include <parselib/parsers/absparser.hpp>

namespace parselib {

namespace myparsers {

class LR_zero : public AbstractParser {
public :

	LR_zero () {} 
	virtual ~LR_zero () {} 
	LR_zero (Grammar grammar) {
		production_rules = grammar.production_rules ;
		unitrelation = grammar.unitrelation ;
	}

	virtual Frame membership (TokenList word) ;

private:
	void build_table();
	void shift_reduce();

};

}

}
