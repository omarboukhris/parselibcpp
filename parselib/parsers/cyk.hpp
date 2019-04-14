#pragma once

#include <parselib/parsers/grammarparser.hpp>
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/datastructure/parsetree.hpp>

namespace parselib {

namespace myparsers {

typedef std::vector<parsetree::Node*> Frame ;
typedef std::vector<Frame> Row ;
typedef std::vector<Row> CYKMatrix ;

Frame operator+(Frame f1, Frame f2) ;

/*!
 * \brief cartesian product between activated production rules in matrix
 * to see if their combination yields a registred production rule
 */
Row cartesianprod (Frame A, Frame B) ;


class CYK {
public :
	
	operations::SequentialParser::ProductionRules production_rules ;
	Grammar::UnitRelation unitrelation ;
	
	CYK (Grammar grammar) ;

	Frame membership (lexer::Lexer::TokenList word) ;
	
	Frame getterminal (lexer::Lexer::Token token) ;
	Frame getAxiomNodes (Frame nodes) ;
	Frame getbinproductions (Row AB) ;

	Frame getrulenames (Frame line) ;
	Frame invUnitRelation (Frame M) ;

	std::string getstrmat (CYKMatrix cykmat) ;
} ;

} // namespace myparsers
	
} // namespace parselib
