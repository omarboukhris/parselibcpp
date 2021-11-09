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

	size_t
		i,  // operations' operators counter
		j ; // token counter
	bool axiomflag ; // whether axiom got read or not
	std::string current_rule ; // name of production rule currently being processed
	
	TokenList grammar ; // grammar's tokens
	TokenList parsedtokens ;
	
	ProductionRules add_operand_to_current_rule (Token tok) ;

	std::string getstr () ;

	inline bool stillparsing () {
		return i < grammar.size() ;
	}

	void check_axiom () ;

	/*!
	 * \brief Read : TERMINAL REGEX
	 */
	void check_for_token () ;
		
	void check_left_side () ;
	
	void check_operators() ;

	void check_right_side () ;

	/// operators on grammar datastructure
	void process_label(std::string label, std::string operand) ;

	void make_list() ;

	void make_regex(int j) ;
		
	/// add to keeper to tell the parser to save this node's content
	void add_to_keeper(int j) ;

	void add_to_str_rules(int j) ;

} ;

} //namespace parsers

} //namespace parselib
