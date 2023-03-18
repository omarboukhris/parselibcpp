#pragma once

#include <queue>

#include <parselib/datastructure/closure_types.h>
#include <parselib/parsers/naiveparsers.hpp>

#include "absparser.hpp"

namespace parselib::parsers {

class LR_zero : public AbstractParser {
public :	

	LR_zero () = default;
	explicit LR_zero (const Grammar& grammar) ;

	~LR_zero () override {}

	Frame membership (const TokenList /*word*/&) override {
		return {};
	}

protected:

	void build_table() ;

	Closure make_closure(Item &current_item) ;

	void shift_reduce();

protected:

	std::vector<Closure> m_graph;

};

}
