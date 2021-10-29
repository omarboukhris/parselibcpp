#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace parsers {

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

	size_t i, j ;
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

} //namespace parsers

} //namespace parselib
