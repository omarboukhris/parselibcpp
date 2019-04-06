#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>

namespace parselib {

namespace operations {

class GenericGrammarTokenizer {

public :
	static lexer::Lexer::PatternsMap grammartokens ;

	static lexer::Lexer::PatternsMap genericgrammarprodrules ;
	
	static lexer::Lexer tokenize (lexer::Lexer tokObj, std::string source, bool verbose=false) {
		tokObj.tokenize (source, verbose) ;
		if (verbose) {
// 			std::cout << tokObj ;
		}
		return tokObj ;
	}
} ;


class SequentialParser {

public :
	typedef lexer::Lexer::TokenList Rule ;
	typedef std::vector<Rule> Rules ;
	typedef std::map <std::string, Rules> ProductionRules ;

	typedef std::vector<std::string> StrList ;
	typedef std::map<std::string, StrList> KeepingList ;

	typedef std::map<std::string, std::string> LabelReplacement ;
	typedef std::map<std::string, LabelReplacement> LabelReplacementMap ;

	bool axiomflag ;
	ProductionRules production_rules ;
	
	StrList strnodes ;
	KeepingList keeper ;
	
	LabelReplacementMap labels ;
	
	lexer::Lexer::TokenList tokens ;

	SequentialParser (lexer::Lexer::TokenList grammar, lexer::Lexer::TokenList parsedtokens) ;


	void parse () ;

private :

	int i, j ;
	std::string current_rule ;
	
	lexer::Lexer::TokenList grammar ;
	lexer::Lexer::TokenList parsedtokens ;

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