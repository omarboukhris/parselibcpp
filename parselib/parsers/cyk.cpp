
#include <parselib/datastructure/parsetree.hpp>

#include "cyk.hpp"

#define DEBUG
inline void DEBUG_OUT(const std::string& x) {
#ifdef DEBUG
	std::cout << "debug output : " << x << std::endl ;
#endif
}

namespace parselib::parsers {

std::string CYK::getstrmat (const CYKMatrix& cykmat) {
	//print html matrix here
	std::stringstream ss ;
    ss << "<html><head><title>CYK Debug</title></head><body><table border='1'>";
	for (const Row& row : cykmat) {
        ss << "<tr>";
        for (const Frame& frame : row) {
            ss << "<td>";
			for (const parsetree::NodePtr& node : frame) {
				ss << node->nodetype + "/" ;
			}
			ss << "</td>" ;
		}
		ss << "</tr>\n" ;
	}
    ss << "</table></body></html>";
    return ss.str() ;
}

Row cartesianprod(const Frame& A, const Frame& B) {
	Row AB = Row() ;
	if (A.empty() || B.empty()) {
		return {} ;
	}
	for (const parsetree::NodePtr& a : A) {
		for (const parsetree::NodePtr& b : B) {
			Frame ab = Frame() ;
			ab.push_back(a);
            ab.push_back(b);
			AB.push_back (ab) ;
		}
	}
	return AB ;
}

CYK::CYK (const Grammar &grammar) {
	production_rules = grammar.production_rules ;
	unitrelation = grammar.unitrelation ;
}

Frame operator+(const Frame& f1, const Frame& f2){
	Frame f = f1 ;
	for (const parsetree::NodePtr& node : f2) {
		f.push_back(node);
	}
	return f ;
}

/*!
 * \brief test membership of a word in a grammar
 * STABLE AF, DON'T TOUCH
 */
Frame CYK::membership (const TokenList &word) {
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
				if (AB.empty()) {
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
//	std::ofstream f ("cykmat.html", std::ofstream::out);
//	f << getstrmat(P) ;
//	f.close();

	if (P[n-1][0].empty()) {
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
			if (!elm.empty()) {
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
    using namespace parsetree;

	Frame terminals = Frame() ;

	for (const auto& item : production_rules) {
		std::string key = item.first ;

		Rules rules = production_rules[key] ;
		for (Rule rule : rules) {

			if (rule.size() == 1 &&
				rule[0].value() == token.type() &&
				rule[0].type() == Token::Terminal
			) {
				NodePtr node = TokenNode::make_token(key, token.value());
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
Frame CYK::getAxiomNodes(const Frame& nodes){
	Frame axiomnodes = Frame() ;
	for (const parsetree::NodePtr& node : nodes) {
		if (node->nodetype == Token::Axiom ||
			node->nodetype == production_rules[Token::Axiom][0][0].value())
		{
			//production_rules[Token::Axiom][0][0].val :
			axiomnodes.push_back (node) ;
		}
	}
	return axiomnodes ;
}

/*!
 * \brief get a list of binarized production rules in a frame
 */
Frame CYK::getbinproductions(const Row& AB, const int MAX) {
	StrVect keys = StrVect() ;
	for (const auto& item : production_rules) {
		keys.push_back(item.first);
	}

	int i = 0;
	Frame bins = Frame() ;
	for (const Frame& line : AB) {
		Frame rulenames = getrulenames (line) ;
		for (const parsetree::NodePtr& rulename : rulenames) {
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
    using namespace parsetree;

	if (line.size() <= 1) {
		return {} ;
	}
	Frame rulenames = Frame() ;
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		for (Rule rule : rules) {
			if (rule.size() <= 1) {
				continue ;
			}

			if (rule[0].value() == line[0]->nodetype &&
				rule[1].value() == line[1]->nodetype) {
				NodePtr node = BinNode::make_bin(key, line[0], line[1]);
				rulenames.push_back (node) ;
			}
		}
	}
	return rulenames ;
}

/*!
 * \brief get inverse unit relation for the parse tree
 */
Frame CYK::invUnitRelation(const Frame& M) {
	// this can be done better
	// refactor unitrelation as inverse unit graph : done
	// use it to probe for non-cyclic unit relations : wip
	// use todo_ queue and processed queues to avoid resp. recursion and cycles : wip
	// note to self : unroll recursion in parsesession : todo
	// once this is figured out debug non-terminals relabeling : todo
	// then go to LR0 : backlog
    using namespace parsetree;

//    StrVect processed = StrVect();
//    Frame todo_ = Frame();

	Frame rulenames = Frame () ;
	for (const parsetree::NodePtr& node : M) {
        if (unitrelation.find(node->nodetype) != unitrelation.end()) {
            for (const auto& unitkey : unitrelation[node->nodetype]) {
                NodePtr nodeOut = UnitNode::make_unit(unitkey, node) ;
                rulenames.push_back (nodeOut) ;
            }
        }
    }
	return rulenames ;
}

} // namespace parselib

