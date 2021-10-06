#pragma once

#include <parselib/parsers/absparser.hpp>


namespace parselib {

namespace myparsers {

Frame operator+(Frame f1, Frame f2) ;

/*!
 * \brief cartesian product between activated production rules in matrix
 * to see if their combination yields a registred production rule
 */
Row cartesianprod (Frame A, Frame B) ;


class CYK : public AbstractParser {
public :

	CYK () {} 
	virtual ~CYK () {}
	CYK (Grammar grammar) ;

	virtual Frame membership (TokenList word) ;

private :
	Frame getterminal (Token token) ;
	Frame getAxiomNodes (Frame nodes) ;
	Frame getbinproductions (Row AB) ;

	Frame getrulenames (Frame line) ;
	Frame invUnitRelation (Frame M) ;

	std::string getstrmat (CYKMatrix cykmat) ;
} ;

} // namespace myparsers
	
} // namespace parselib
