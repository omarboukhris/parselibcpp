#pragma once

#include <parselib/parsers/absparser.hpp>


namespace parselib {

namespace parsers {

Frame operator+(Frame f1, Frame f2) ;

/*!
 * \brief cartesian product between activated production rules in matrix
 * to see if their combination yields a registred production rule
 */
Row cartesianprod (Frame A, Frame B) ;

/*!
 * \brief The CYK class implements an optimized CYK parser
 * Grammar needs to be 2nf
 */
class CYK : public AbstractParser {
public :

	CYK () {} 
	virtual ~CYK () {}

	/*!
	 * \brief CYK constructor
	 * \param grammar  Grammar to use for parsing
	 */
	CYK (Grammar grammar) ;

	/*!
	 * \brief membership method checks if word is part of the language
	 * \param word  is a list of tokens
	 * \return Frame containing parse trees
	 */
	virtual Frame membership (TokenList word) ;

private :

	/*!
	 * \brief get terminal nodes for the cyk table + parse tree
	 * used for initialization
	 * \param token to process
	 * \return Frame containing the appropriate nodes
	 */
	Frame getterminal (Token token) ;

	/*!
	 * \brief returns all well parsed nodes with key "axiom"
	 * on the last frame of the CYKMatrix
	 * \param nodes  Frame containing final nodes of the CYK matrix
	 * \return Frame containing the right axiom nodes
	 */
	Frame getAxiomNodes (Frame nodes) ;

	/*!
	 * \brief getbinproductions return frame with binary production rules
	 * \param AB CYK matrix row
	 * \return Frame containing binary rules
	 */
	Frame getbinproductions (Row AB) ;

	/*!
	 * \returns a list of valid nodes corresponding
	 * to the rules being inspected
	 * \param line being inspected
	 * \return frame with the rules
	 */
	Frame getrulenames (Frame line) ;

	/*!
	 * \brief get inverse unit relation for the parse tree
	 * \param M Frame to get inverse unit relation from
	 * \return Frame containing inverse unit relation production rules
	 */
	Frame invUnitRelation (Frame M) ;

	/*!
	 * \brief getstrmat grossly prints CYK matrix into a string
	 * \param cykmat  CYK matrix
	 * \return string representing the matrix
	 */
	std::string getstrmat (CYKMatrix cykmat) ;
} ;

} // namespace parsers
	
} // namespace parselib
