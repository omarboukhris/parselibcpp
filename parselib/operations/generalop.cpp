
#include "generalop.hpp"

namespace parselib {

namespace grammaroperators {

PatternsMap GenericGrammarTokenizer::grammartokens = {
	//PREPROCESSOR
	{R"(\%import "(.+)(/([^/]+))?\.grm")",	"IMPORT"},

	//KEYWORDS
	{R"((//|\;|\-\-).*)",             "LINECOMMENT"},
	{R"(''|"")",                       Token::Empty},
	{Token::Axiom,                     Token::Axiom},

	// SPECIAL OPERATORS
    {R"((\_\_list\_\_|\[\]))",       "LIST"}, // deprecated once the listop is implemented
    // list_node=list(elm_node, sep=tok.)
    {R"(\w+\=list\((\w+\=)?\w+(\.)?\, \w+\.\))", "LISTOP"}, // separator
	{"\\!",                          "EXCL"},
	{R"(([a-zA-Z_]\w*=)?s\:)",        "STR"},
	{R"(\(".*"\)|\('.*'\))",        "REGEX"},
	{R"((\->|\=))",                 "EQUAL"},
	{"\\,",                         "COMMA"},
	{"\\|",                            "OR"},
	{"\\(",                          "LPAR"},
	{"\\)",                          "RPAR"},

	//OPERANDS
	{R"(([a-zA-Z_]\w*=)?[a-zA-Z0-9_]\w*\.)",    Token::Terminal},
	{"([a-zA-Z_]\\w*=)?[a-zA-Z0-9_]\\w*",    Token::NonTerminal}
} ;

PatternsMap GenericGrammarTokenizer::genericgrammarprodrules = {
	{"LINECOMMENT",                                     "LINECOMMENT"},
	{"AXIOM EQUAL NONTERMINAL",                         Token::Axiom},
	{"TERMINAL REGEX",                                  "TOKEN"},
	{"NONTERMINAL EQUAL",                               "LSIDE"},
	{"EXCL|STR|LIST|AREGEX|TERMINAL|NONTERMINAL|EMPTY|LISTOP", "RSIDE"},
	{"OR", "OR"},
} ;

/*!
 * \brief eliminates duplicate rules in a grammar
 * \param grammar : grammar in
 * \returns grammar : grammar out
 */
Grammar eliminatedoubles (Grammar grammar) {

	ProductionRules production_rules = ProductionRules () ;
	for (const auto& item : grammar.production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		
		Rules uniquerules = Rules() ;
		for (const Rule& rule : rules) {
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
bool checkunique (const Rules& uniquerules, const Rule& rule) {
    return std::any_of(uniquerules.begin(), uniquerules.end(), [&] (const Rule& r) {
        return r == rule;
    });
}

}



