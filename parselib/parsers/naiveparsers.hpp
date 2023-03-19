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

	SequentialParser (const TokenList &grammar, const TokenList &parsedtokens) ;

	void parse () ;

	friend std::ostream & operator<< (std::ostream & out, SequentialParser str) {
		out << str.getstr() ;
		return out ;
	}


private :

	size_t
		op_count,  // operations' operators counter
		tok_count ; // token counter
	bool axiomflag ; // whether axiom got read or not
	std::string current_rule ; // name of production rule currently being processed
	
	TokenList grammar ; // grammar's tokens
	TokenList parsedtokens ;
	
	ProductionRules add_operand_to_current_rule (const Token& tok) ;

	std::string getstr () ;

	inline bool stillparsing () {
		return op_count < grammar.size() ;
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
	void process_label(const std::string& label, const std::string& operand) ;

	void make_list() ;

	void make_regex(size_t j) ;
		
	/// add to keeper to tell the parser to save this node's content
	void add_to_keeper(size_t j) ;

	void add_to_str_rules(size_t j) ;

} ;

} //namespace parsers

} //namespace parselib
