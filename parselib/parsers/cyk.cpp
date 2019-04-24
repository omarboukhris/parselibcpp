
#include <parselib/parsers/cyk.hpp>
#include <parselib/datastructure/parsetree.hpp>
#include <boost/iterator/iterator_concepts.hpp>

#define DEBUG
inline void DEBUG_OUT(std::string x) {
#ifdef DEBUG
	std::cout << "debug output : " << x << std::endl ;
#endif
}

namespace parselib {

namespace myparsers {

std::string CYK::getstrmat (CYKMatrix cykmat) {
	//print matrix here
	std::string ss = "" ;
	for (Row row : cykmat) {
		for (Frame frame : row) {
			for (parsetree::Node* node : frame) {
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
	for (parsetree::Node* a : A) {
		for (parsetree::Node* b : B) {
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
	for (parsetree::Node* node : f2) {
		f.push_back(node);
	}
	return f ;
}

/*!
 * \brief test membership of a word in a grammar
 * STABLE AF, DON'T TOUCH
 */
Frame CYK::membership (lexer::Lexer::TokenList word) {
	size_t n = word.size() ;
	CYKMatrix P = CYKMatrix(n, Row (n, Frame())) ;
	
	for (size_t i = 0 ; i < n ; i++) {
		P[0][i] = getterminal (word[i]) ;
		P[0][i] = P[0][i] + invUnitRelation (P[0][i]) ;
	}
// 	std::cout << getstrmat(P) ;
	
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
	}
// 	std::cout << getstrmat(P) ;

	if (P[n-1][0].size() == 0) {
		return Frame() ; // try returning the broken nodes
	}
	return getAxiomNodes (P[n-1][0]) ;
}

/*!
 * \brief get terminal nodes for the cyk table + parse tree
 */
Frame CYK::getterminal (lexer::Lexer::Token token) {
	SequentialParser::StrList keys = SequentialParser::StrList() ;
	for (auto item : production_rules) {
		keys.push_back(item.first);
	}

	Frame terminals = Frame() ;
	for (std::string key : keys) {

		SequentialParser::Rules rules = production_rules[key] ;
		for (SequentialParser::Rule rule : rules) {

			if (rule.size() == 1 && rule[0].second == "TERMINAL" && rule[0].first == token.second) {
				parsetree::Node* node = (parsetree::Node*) new parsetree::TokenNode (key, token.first) ;
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
	for (parsetree::Node* node : nodes) {
		if (node->nodetype == "AXIOM") { //production_rules["AXIOM"][0][0].val :
			axiomnodes.push_back (node) ;
		}
	}
	return axiomnodes ;
}

/*!
 * \brief get a list of binarized production rules in a frame
 */
Frame CYK::getbinproductions(Row AB) {
	SequentialParser::StrList keys = SequentialParser::StrList() ;
	for (auto item : production_rules) {
		keys.push_back(item.first);
	}

	Frame bins = Frame() ;
	for (Frame line : AB) {
		Frame rulenames = getrulenames (line) ;
		for (parsetree::Node* rulename : rulenames) {
			//add node for parse tree here
			bins.push_back (rulename) ;
		}
	}
// 	#return list (set(bins))
 	return bins ;
}

/*!
 * \returns a list of valid nodes corresponding 
 * to the rules being inspected
 */
Frame CYK::getrulenames(Frame line) {
	if (line.size() == 0) {
		return Frame() ;
	}
	Frame rulenames = Frame() ;
	for (auto item : production_rules) {
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;
 		for (SequentialParser::Rule rule : rules) {
			if (rule.size() == 1) {
				continue ;
			}

			if (rule[0].first == line[0]->nodetype &&
				rule[1].first == line[1]->nodetype) {
				parsetree::Node* node = new parsetree::BinNode (key, line[0], line[1]) ;
				rulenames.push_back (node) ;
			}
		}
	}
	return rulenames ;
}

/// \brief get inverse unit relation for the parse tree
Frame CYK::invUnitRelation(Frame M) {
	Frame rulenames = Frame () ;
	for (parsetree::Node* node : M) {
		for (auto item : unitrelation) {
			std::string key = item.first ;
			SequentialParser::StrList units = item.second ;
			if (std::find(units.begin(), units.end(), node->nodetype) != units.end()) {
				parsetree::Node* nodeOut = /*(parsetree::Node*)*/ new parsetree::UnitNode (key, node) ;
				rulenames.push_back (nodeOut) ;
			}
		}
	}
	return rulenames ;
}

} // namespace myparsers
	
} // namespace parselib

