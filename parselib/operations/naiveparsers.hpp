#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>

namespace parselib {

typedef TokenList Rule ;
typedef std::vector<Rule> Rules ;
typedef std::map <std::string, Rules> ProductionRules ;

typedef std::vector<std::string> StrList ;
typedef std::map<std::string, StrList> KeepingList ;

typedef std::map<std::string, std::string> LabelReplacement ;
typedef std::map<std::string, LabelReplacement> LabelReplacementMap ;

namespace operations {

class GenericGrammarTokenizer {

public :
	static PatternsMap grammartokens ;

	static PatternsMap genericgrammarprodrules ;
	
	static lexer::Lexer tokenize (lexer::Lexer tokObj, std::string source, bool verbose=false) {
		tokObj.tokenize (source, verbose) ;
		return tokObj ;
	}
} ;


class SequentialParser {

public :

	ProductionRules production_rules ;
	
	StrList strnodes ;
	KeepingList keeper ;
	
	LabelReplacementMap labels ;
	
	TokenList tokens ;

	SequentialParser (TokenList grammar, TokenList parsedtokens) ;

	void parse () ;

	friend std::ostream & operator<< (std::ostream & out, SequentialParser str) {
		out << str.getstr() ;
		return out ;
	}


private :

	int i, j ;
	bool axiomflag ;
	std::string current_rule ;
	
	TokenList grammar ;
	TokenList parsedtokens ;
	
	ProductionRules addoperandtocurrentrule (Token tok) ;

	std::string getstr () ;

	bool stillparsing () ;
	
	void checkaxiom () ;

	/*!
	 * \brief Read : TERMINAL REGEX
	 */
	void checkfortoken () ;
		
	void checkleftside () ;
	
	void checkoperators() ;

	void checkrightside () ;

	/// operators on grammar datastructure
	void processlabel(std::string label, std::string operand) ;

	void makelist() ;

	void makeregex(int j) ;
		
	/// add to keeper to tell the parser to save this node's content
	void addtokeeper(int j) ;

	void addtostr(int j) ;

} ;

} //namespace operations

} //namespace parselib
