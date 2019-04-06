#include <parselib/operations/naiveparsers.hpp>

namespace parselib {

namespace operations {

lexer::Lexer::PatternsMap GenericGrammarTokenizer::grammartokens = {
	//PREPROCESSOR
	{"\\%(import|include) \"(.+)/([^/]+)\\.grm\"",	"IMPORT"},
	
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

lexer::Lexer::PatternsMap GenericGrammarTokenizer::genericgrammarprodrules = {
	{"LINECOMMENT",						          "LINECOMMENT"},
	{"AXIOM EQUAL NONTERMINAL",							"AXIOM"},
	{"TERMINAL REGEX",									"TOKEN"},
	{"NONTERMINAL EQUAL",								"LSIDE"},
	{"EXCL|STR|LIST|AREGEX|TERMINAL|NONTERMINAL|EMPTY",	"RSIDE"},
	{"OR", "OR"},
// 	{"LCRCH",		"LCRCH"},
// 	{"RCRCH",		"RCRCH"}
} ;
	
SequentialParser::SequentialParser (
	lexer::Lexer::TokenList grammar, 
	lexer::Lexer::TokenList parsedtokens) 
: i(0), j(0), current_rule(std::string("")) {

	axiomflag = true ;

	production_rules = ProductionRules() ;

	strnodes = StrList () ;

	keeper = KeepingList() ;
	keeper["all"] = StrList() ;

	labels = LabelReplacementMap() ;

	tokens = lexer::Lexer::TokenList () ;

	this->grammar = grammar ;
	this->parsedtokens = parsedtokens ;
}

void SequentialParser::parse () {
	while (stillparsing()) {
		checkaxiom () ;
		checkleftside() ;
		checkrightside() ;
		checkoperators () ;
		checkfortoken() ;
	}
}

bool SequentialParser::stillparsing () {
	return i < grammar.size() ; //grammar is vector of something
}

void SequentialParser::checkaxiom () {
	int 
		i = this->i, 
		j = this->j ;
	
	if (!i < grammar.size()) {
		return ;
	}
	
	// grammar(i).first = value,
	//			 .second = type
	if (grammar[i].second == std::string("AXIOM") && axiomflag ) {
		lexer::Lexer::Token axiom = parsedtokens[j+2] ;
		production_rules["AXIOM"] = {{axiom}} ;
		axiomflag = false ;
		i += 1 ;
		j += 3 ;
	}
	this->i = i ;
	this->j = j ;
}

void SequentialParser::checkfortoken () {
	int 
		i = this->i, 
		j = this->j ;
	
	if (!i < grammar.size()) {
		return ;
	}
	
	if (grammar[i].second == std::string("TOKEN")) {
		
		//eliminate the dot @token value
		std::string label = utils::cleanIfTerminal(parsedtokens[j].first) ; 
		//eliminate the parentheses+quotes
		std::string regex = utils::cleanRegex(parsedtokens[j+1].first) ; 

		tokens.push_back({regex, label}) ;
		i += 1 ;
		j += 2 ;
	}
	this->i = i ;
	this->j = j ;
}

void SequentialParser::checkleftside () {
	int 
		i = this->i, 
		j = this->j ;
	
	if (!i < grammar.size()) {
		return ;
	}
	if (grammar[i].second == "LSIDE") {
		current_rule = parsedtokens[j].first ;
		
		// if current_rule in production_rules.keys()
		if (production_rules.find (current_rule) == production_rules.end()) {
			production_rules[current_rule] = Rules() ;
		}
		
		i += 1 ;
		j += 2 ;
	}
	this->i = i ;
	this->j = j ;
}

void SequentialParser::checkoperators () {
	int 
		i = this->i, 
		j = this->j ;
	
	if (!i < grammar.size()) {
		return ;
	}

	if (grammar[i].second == std::string("OR") && 
		parsedtokens[j].second == std::string("OR")) {

		//add a new rule (list of tokens)
		production_rules[current_rule].push_back(
			Rule ()
		) ;
		j += 1 ;
		i += 1 ;
	}
	if (grammar[i].second == std::string("LINECOMMENT") &&
		parsedtokens[j].second == std::string("LINECOMMENT")) {
		j += 1 ;
		i += 1 ;
	}
	this->i = i ;
	this->j = j ;
}

void SequentialParser::checkrightside() {
	int 
		i = this->i, 
		j = this->j ;
	
	if (!i < grammar.size()) {
		return ;
	}

	while (i < grammar.size() && 
		grammar[i].second == std::string("RSIDE")) {

		if (parsedtokens[j].second == std::string("TERMINAL")) {
			parsedtokens[j].first = utils::cleanIfTerminal(parsedtokens[j].first) ;
		}

		if (parsedtokens[j].second == std::string("STR")) {
			addtostr(j) ;
			addtokeeper(j) ;
		} else if (parsedtokens[j].second == std::string("LIST")) {
			makelist() ;
		} else if (parsedtokens[j].second == std::string("AREGEX")) {
			makeregex(j) ;
		} else if (parsedtokens[j].second == std::string("EXCL")) {
			
			// naming process
			std::string label = parsedtokens[j+1].first ;
			label = utils::cleanIfTerminal (label) ;
			parsedtokens[j].first = label ;

			processlabel (label, label) ;
			addtokeeper(j) ;
		} else {
			if (parsedtokens[j].first.find('=') != std::string::npos) {
				//naming process
				utils::StrList out = utils::split(parsedtokens[j].first, std::string("=")) ;
				std::string label, operand ;
				
				if (out.size() == 2) {
					label = out[0] ;
					operand = out[1] ;
				} else {
					//error happened
					return ;
				}

				parsedtokens[j].first = operand ;

				processlabel(label, operand) ;
			}
			production_rules[current_rule].back().push_back(parsedtokens[j]) ;
		}
		i += 1 ;
		j += 1 ;
	}

	this->i = i ;
	this->j = j ;
}

void SequentialParser::processlabel(std::__cxx11::string label, std::__cxx11::string operand){
	//currentrule in labels.keys()
	if (labels.find (current_rule) != labels.end()) {
		labels[current_rule][operand] = label ;
	} else {
		labels[current_rule] = LabelReplacement() ;
		labels[current_rule][operand] = label ;
	}

	if (keeper.find(current_rule) != keeper.end()) {
		keeper[current_rule].push_back(label) ;
	} else {
		keeper[current_rule] = StrList() ;
		keeper[current_rule].push_back(label) ;
	}
}

void SequentialParser::makelist(){
	lexer::Lexer::Token thisnode (std::string("NONTERMINAL"), current_rule) ;
	lexer::Lexer::Token eps (std::string("EMPTY"), std::string("''")) ;
	production_rules[current_rule].back() = {thisnode, thisnode} ;
	production_rules[current_rule].push_back({eps}) ;
}

void SequentialParser::makeregex(int j) {
	std::string regex= utils::escapeRegex(
		parsedtokens[j].first.substr(
			1, 
			parsedtokens[j].first.length()-2
		) //eliminate the ["..."]
	) ;

	std::string label = 
		std::string("__") + current_rule + 
		std::string("[") + regex + std::string("]__") ;
	
	tokens.push_back(
		lexer::Lexer::Token(regex, label)
	) ; 

	lexer::Lexer::Token thisnode ("TERMINAL", label) ;
	production_rules[current_rule].back().push_back(thisnode) ;
}

void SequentialParser::addtokeeper(int j) {
	if (keeper.find(current_rule) != keeper.end()) {
		keeper[current_rule].push_back(parsedtokens[j + 1].first) ;
	} else {
		keeper[current_rule] = StrList() ;
		keeper[current_rule].push_back(parsedtokens[j + 1].first) ;
	}
}

void SequentialParser::addtostr(int j) {
	std::string nodename = parsedtokens[j + 1].first ;
	if (parsedtokens[j + 1].second == "TERMINAL") {
		nodename = nodename.substr(0, nodename.length()-1) ; //eliminate last char '.'
	}
	// add to strnodes
	if (std::find(strnodes.begin(), strnodes.end(), nodename) != strnodes.end()) {
		strnodes.push_back(nodename) ;
	}
}


} //namespace operations


} //namespace parselib 



