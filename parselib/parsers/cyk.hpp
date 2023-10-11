#pragma once

#include <parselib/parsers/absparser.hpp>


namespace parselib::parsers {

Frame operator+(const Frame& f1, const Frame& f2) ;

/*!
 * \brief cartesian product between activated production rules in matrix
 * to see if their combination yields a registred production rule
 */
Row cartesianprod (const Frame& A, const Frame& B) ;

/*!
 * \brief The CYK class implements an optimized CYK parser
 * Grammar needs to be 2nf
 */
class CYK : public AbstractParser {
public :

	CYK () = default;
	~CYK () override = default;

    /// \brief CYK constructor
	explicit CYK (const Grammar &grammar) ;

    /// \brief membership method checks if word is part of the language
	Frame membership (const TokenList &word) override ;

private :

    /// \brief get terminal nodes for the cyk table + parse tree
	Frame getterminal (Token token) ;

    /// \brief returns all well parsed nodes with key "axiom"
	Frame getAxiomNodes (const Frame& nodes) ;

    /// \brief returns last well parsed nodes in the CYKMatrix
	static Frame getBrokenNodes (const CYKMatrix &mat) ;

    /// \brief getbinproductions return frame with binary production rules
	Frame getbinproductions (const Row& AB, int MAX = 10) ;

    /// \brief gets a list of valid nodes corresponding
	Frame getrulenames (Frame line) ;

    /// \brief get inverse unit relation for the parse tree
	Frame invUnitRelation (const Frame& M) ;

    /// \brief getstrmat grossly prints CYK matrix into a string
	static std::string getstrmat (const CYKMatrix& cykmat) ;
} ;

} // namespace parselib
