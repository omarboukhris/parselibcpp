
#include <parselib/datastructure/parsetree.hpp>

#include "cyk.hpp"

#define DEBUG
inline void DEBUG_OUT(std::string x) {
#ifdef DEBUG
	std::cout << "debug output : " << x << std::endl ;
#endif
}

namespace parselib {

namespace parsers {

std::string CYK::getstrmat (CYKMatrix cykmat) {
	//print matrix here
	std::string ss = "" ;
	for (Row row : cykmat) {
		for (Frame frame : row) {
			for (parsetree::NodePtr node : frame) {
				ss += node->nodetype + ":" ;
			}
			ss += "|" ;
		}
		ss += "\n" ;
	}
	return ss ;
}

Row cartesianprod(Frame A, Frame B) {
	Row AB = Row() ;
	if (A.size() == 0 || B.size() ==  0) {
		return Row() ;
	}
	for (parsetree::NodePtr a : A) {
		for (parsetree::NodePtr b : B) {
			Frame ab = Frame() ;
			ab.push_back(a); ab.push_back(b);
			AB.push_back (ab) ;
		}
	}
	return AB ;
}

CYK::CYK (Grammar grammar) {
	production_rules = grammar.production_rules ;
	unitrelation = grammar.unitrelation ;
}

Frame operator+(Frame f1, Frame f2){
	Frame f = f1 ;
	for (parsetree::NodePtr node : f2) {
		f.push_back(node);
	}
	return f ;
}

/*!
 * \brief test membership of a word in a grammar
 * STABLE AF, DON'T TOUCH
 */
Frame CYK::membership (TokenList word) {
	size_t n = word.size() ;
	CYKMatrix P = CYKMatrix(n, Row (n, Frame())) ;

	for (size_t i = 0 ; i < n ; i++) {
		P[0][i] = getterminal (word[i]) ;
		P[0][i] = P[0][i] + invUnitRelation (P[0][i]) ;
	}
// 	std::cout << getstrmat(P) ;
	utils::ProgressBar pbar (n) ;
	for (size_t l = 1 ; l < n ; l++) {

		for (size_t i = 0 ; i < n-l ; i++) {

			for (size_t k = 0 ; k < l ; k++) {

				Frame
					B = P[l-k-1][k+i+1],
					A = P[k][i] ;
				Row AB = cartesianprod (A, B) ;
				if (AB.size() == 0) {
					continue ;
				}

				Frame rulenames = getbinproductions (AB) ;
				P[l][i] = P[l][i] + rulenames ;
				P[l][i] = P[l][i] + invUnitRelation (rulenames) ;
			}
		}
		pbar.update(l);
	}
	std::cout << "\r" << std::flush ;
	// std::cout << getstrmat(P) ;

	if (P[n-1][0].size() == 0) {
		return getBrokenNodes(P);
	}
	return getAxiomNodes (P[n-1][0]) ;
}

/*!
 * \brief returns last well parsed nodes in the CYKMatrix
 */
Frame CYK::getBrokenNodes (const CYKMatrix &mat) {
	Frame output;
	size_t i = 0, o_j = 0;
	for (const Row& line: mat) {
		size_t j = 0;
		for (const Frame& elm: line) {
			if (elm.size() > 0) {
				output = elm;
				o_j = j ;
			}
			j++ ;
		}
		i++ ;
	}
	if (o_j)
		output.push_back(mat.at(0).at(o_j-1).at(0));

	return output;
}


/*!
 * \brief get terminal nodes for the cyk table + parse tree
 */
Frame CYK::getterminal (Token token) {
	Frame terminals = Frame() ;

	for (auto item : production_rules) {
		std::string key = item.first ;

		Rules rules = production_rules[key] ;
		for (Rule rule : rules) {

			if (rule.size() == 1 &&
				rule[0].value() == token.type() &&
				rule[0].type() == "TERMINAL"
			) {
				parsetree::NodePtr node = std::make_shared<parsetree::TokenNode>(
					parsetree::TokenNode (key, token.value())) ;
				terminals.push_back (node) ;
			}
		}
	}
	return terminals ;
}

/*!
 * \brief returns all well parsed nodes with key "axiom"
 * on the last frame of the CYKMatrix
 */
Frame CYK::getAxiomNodes(Frame nodes){
	Frame axiomnodes = Frame() ;
	for (parsetree::NodePtr node : nodes) {
		if (node->nodetype == "AXIOM" ||
			node->nodetype == production_rules["AXIOM"][0][0].value())
		{
			//production_rules["AXIOM"][0][0].val :
			axiomnodes.push_back (node) ;
		}
	}
	return axiomnodes ;
}

/*!
 * \brief get a list of binarized production rules in a frame
 */
Frame CYK::getbinproductions(Row AB, const int MAX) {
	StrList keys = StrList() ;
	for (auto item : production_rules) {
		keys.push_back(item.first);
	}

	int i = 0;
	Frame bins = Frame() ;
	for (Frame line : AB) {
		Frame rulenames = getrulenames (line) ;
		for (parsetree::NodePtr rulename : rulenames) {
			//add node for parse tree here
			if (i++<MAX)
			bins.push_back (rulename) ;
		}
	}
// 	#return list (set(bins))
	return bins ;
}

/*!
 * \brief returns a list of valid nodes corresponding
 * to the rules being inspected in frame "line"
 */
Frame CYK::getrulenames(Frame line) {
	if (line.size() <= 1) {
		return Frame() ;
	}
	Frame rulenames = Frame() ;
	for (auto item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		for (Rule rule : rules) {
			if (rule.size() <= 1) {
				continue ;
			}

			if (rule[0].value() == line[0]->nodetype &&
				rule[1].value() == line[1]->nodetype) {
				parsetree::NodePtr node = std::make_shared<parsetree::BinNode>(
					parsetree::BinNode (key, line[0], line[1])) ;
				rulenames.push_back (node) ;
			}
		}
	}
	return rulenames ;
}

/*!
 * \brief get inverse unit relation for the parse tree
 */
Frame CYK::invUnitRelation(Frame M) {
	Frame rulenames = Frame () ;
	for (parsetree::NodePtr node : M) {
		for (auto item : unitrelation) {
			std::string key = item.first ;
			StrList units = item.second ;
			if (std::find(units.begin(), units.end(), node->nodetype) != units.end()) {
				parsetree::NodePtr nodeOut = std::make_shared<parsetree::UnitNode>(
					parsetree::UnitNode (key, node)) ;
				rulenames.push_back (nodeOut) ;
			}
		}
	}
	return rulenames ;
}

} // namespace parsers

} // namespace parselib

