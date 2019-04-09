#include <parselib/operations/naiveparsers.hpp>

using namespace std ;

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
	
	if (i >= grammar.size()) {
		return ;
	}
	
	// grammar(i).first = value,
	//			 .second = type
	if (grammar[i].second == std::string("AXIOM") && axiomflag ) {
		lexer::Lexer::Token axiom = parsedtokens[j+2] ;
		production_rules["AXIOM"] = {{axiom}} ;
// 		cout << production_rules["AXIOM"][0][0].first << endl ;
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
	
	if (i >= grammar.size()) {
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
	
	if (i >= grammar.size()) {
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
	
	if (i >= grammar.size()) {
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
	
	if (i >= grammar.size()) {
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
			makeregex(j) ; //couille ici
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
			
 			production_rules = addoperandtocurrentrule(parsedtokens[j]) ;
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
	std::string regex = 
	//utils::escapeRegex( //escapeRegex gives weird results
	parsedtokens[j].first.substr(
		1,
		parsedtokens[j].first.length()-2
	) ; //eliminate the ["..."]
	// 	) ;
	
	std::string label = 
		std::string("__") + current_rule + 
		std::string("[") + regex + std::string("]__") ;
	
	tokens.push_back(
		lexer::Lexer::Token(regex, label)
	) ; 

	lexer::Lexer::Token thisnode (label, "TERMINAL") ;
	production_rules = addoperandtocurrentrule(thisnode) ;
}

SequentialParser::ProductionRules SequentialParser::addoperandtocurrentrule(lexer::Lexer::Token tok) {
	// adding an operand to the current running rule
	if (production_rules[current_rule].size() != 0) {
		production_rules[current_rule].back().push_back(tok) ;
	} else {
		production_rules[current_rule].push_back({tok}) ;
	}
	return production_rules ;
}

void SequentialParser::addtokeeper(int j) {
	string entry = utils::cleanIfTerminal(parsedtokens[j + 1].first) ;
	if (keeper.find(current_rule) != keeper.end()) {
		if (std::find (keeper[current_rule].begin(), keeper[current_rule].end(), entry) == keeper[current_rule].end()) {
			keeper[current_rule].push_back(entry);
		}
	} else {
		keeper[current_rule] = StrList() ;
		keeper[current_rule].push_back(entry) ;
	}
}

void SequentialParser::addtostr(int j) {
	std::string nodename = utils::cleanIfTerminal(parsedtokens[j + 1].first) ;
	// add to strnodes
	if (std::find(strnodes.begin(), strnodes.end(), nodename) == strnodes.end()) {
		strnodes.push_back(nodename) ;
	}
}

/// \brief Screaming results for debug resons or verbose
string SequentialParser::getstr () {
	string text_rule = "" ;

	for (auto item : production_rules) {
		string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		
		text_rule += "\nRULE " + key + " = [\n\t" ;
		
		SequentialParser::StrList rule_in_a_line = SequentialParser::StrList () ;
		
		for (SequentialParser::Rule rule : rules) {
			SequentialParser::StrList ruletxt = SequentialParser::StrList () ;
			for (lexer::Lexer::Token opr : rule) {
				ruletxt.push_back(opr.second+"("+opr.first+")");
			}
			string thisrule = utils::join(ruletxt, " ") ;
			rule_in_a_line.push_back(thisrule);
		}
		text_rule += utils::join(rule_in_a_line, "\n\t") + "]" ;		
	}
	
	text_rule += "\n\n" ;
	
	text_rule += "LABELS = [\n" ;
	for (auto item : labels) {
		string key = item.first ;
		SequentialParser::LabelReplacement labmap = item.second ;
		text_rule += key + " {\n" ;
		for (auto lab : labmap) {
			text_rule += "\t" + lab.first + " : " + lab.second + "\n" ;
		}
		text_rule += "}\n" ;
	}
	text_rule += "]\n" ;

	text_rule += "STRUCT = [\n" ;
	for (auto item : keeper) {
		string key = item.first ;
		SequentialParser::StrList listkeep = item.second ;
		text_rule += "" + key + " {\n\t" ;
		text_rule += utils::join(listkeep, "\n\t") ;
		text_rule += "}\n" ;
	}
	text_rule += "\n]\n\n" ;
	
	text_rule += "STRNODE = [\n" + utils::join(strnodes, "") + "\n]\n\n" ;

	for (lexer::Lexer::Token tok : tokens) {
		string label = tok.second ;
		string regx = tok.first ;
		text_rule += "TOKEN " + label + " = regex('" + regx + "')\n" ;
	}

	return text_rule ;
}

} //namespace operations


} //namespace parselib 



