#pragma once

#include <queue>

#include <parselib/datastructure/closure_types.h>
#include <parselib/parsers/naiveparsers.hpp>

#include "absparser.hpp"

namespace parselib {

namespace myparsers {

class LR_zero : public AbstractParser {
public :	

	LR_zero () {}
	LR_zero (Grammar grammar) ;

	virtual ~LR_zero () {}

	virtual Frame membership (TokenList word) {
		return Frame();
	}

private:

	void build_table() ;

	Closure make_closure(Item &current_item) ;

	void shift_reduce();

};

}

}
