
#include "generalop.hpp"

namespace parselib {

namespace grammaroperators {

PatternsMap GenericGrammarTokenizer::grammartokens = {
	//PREPROCESSOR
	{"\\%(import|include) \"(.+)(/([^/]+))?\\.grm\"",	"IMPORT"},

	//KEYWORDS
	{"(//|\\;).*",						"LINECOMMENT"},
	{"\'\'|\"\"",						"EMPTY"},
	{"AXIOM",							"AXIOM"},

	// SPECIAL OPERATORS
	{"(\\_\\_list\\_\\_|\\[\\])",		"LIST"},
	{"\\!",								"EXCL"},
	{"s\\:",							"STR"},
	{"\\(\".*\"\\)|\\(\'.*\'\\)",		"REGEX"},
	{"\".*\"|\'.*\'",					"AREGEX"}, //a for anonymous
	{"(\\->|\\=)",						"EQUAL"},
	{"\\,",								"COMMA"},
	{"\\|",								"OR"},
	{"\\(",								"LPAR"},
	{"\\)",								"RPAR"},
// 	{"\[",								"LCRCH"},
// 	{"\]",								"RCRCH"},

	//OPERANDS
	{"([a-zA-Z_]\\w*=)?[a-zA-Z0-9_]\\w*\\.",	"TERMINAL"},
	{"([a-zA-Z_]\\w*=)?[a-zA-Z0-9_]\\w*",		"NONTERMINAL"}
} ;

PatternsMap GenericGrammarTokenizer::genericgrammarprodrules = {
	{"LINECOMMENT",						          "LINECOMMENT"},
	{"AXIOM EQUAL NONTERMINAL",							"AXIOM"},
	{"TERMINAL REGEX",									"TOKEN"},
	{"NONTERMINAL EQUAL",								"LSIDE"},
	{"EXCL|STR|LIST|AREGEX|TERMINAL|NONTERMINAL|EMPTY",	"RSIDE"},
	{"OR", "OR"},
// 	{"LCRCH",		"LCRCH"},
// 	{"RCRCH",		"RCRCH"}
} ;

/*!
 * \brief eliminates duplicate rules in a grammar
 * \param grammar : grammar in
 * \returns grammar : grammar out
 */
Grammar eliminatedoubles (Grammar grammar) {

	ProductionRules production_rules = ProductionRules () ;
	for (auto item : grammar.production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		
		Rules uniquerules = Rules() ;
		for (Rule rule : rules) {
			bool exist = checkunique (uniquerules, rule) ;
			if (!exist) {
				uniquerules.push_back(rule);
			}
		}
		production_rules[key] = uniquerules ;
	}
	grammar.production_rules = production_rules ;
	return grammar ;

}

}

/*!
 * \brief check if rule already exists
 * \param uniquerules : (Rules) list of unique rules
 * \param rule : rule to check
 * \returns true or false (bool)
 */
bool checkunique (Rules uniquerules, Rule rule) {
	for (Rule r : uniquerules) {
		if (samerule (r, rule)) {
			return true ;
		}
	}
	return false ;
}
	
/*!
 * \brief check is rule a & b are the same
 * \param a
 * \param b : rules to compare
 * \returns bool
 */
bool samerule (Rule rulea, Rule ruleb) {
	if (rulea.size() == ruleb.size()) {
		Rule::iterator opa = rulea.begin() ;
		Rule::iterator opb = ruleb.begin() ; 
		
		while (opa != rulea.end() && opb != ruleb.end()) {
			if (!(opa->value() == opb->value() &&
				opa->type() == opb->type())) {
				return false ;
			}
			opa++ ; opb++ ;
		}
		return true ;
	} else {
		return false ;
	}
}

}



