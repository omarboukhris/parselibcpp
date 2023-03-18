#pragma once

#include <parselib/datastructure/common_types.h>
#include <parselib/parsers/grammarparser.hpp>
#include <parselib/datastructure/parsetree.hpp>

namespace parselib {

typedef std::vector<parsetree::NodePtr> Frame ;
typedef std::vector<Frame> Row ;
typedef std::vector<Row> CYKMatrix ;

namespace parsers {

/*!
 * \brief The AbstractParser class
 */
class AbstractParser {
protected:

	ProductionRules production_rules ;
	Grammar::UnitRelation unitrelation ;

public:
	AbstractParser () = default;
	virtual ~AbstractParser () = default;
	explicit AbstractParser (const Grammar& grammar) {
		production_rules = grammar.production_rules ;
		unitrelation = grammar.unitrelation ;
	}

	virtual Frame membership (const TokenList &word) = 0;

};

}

}
