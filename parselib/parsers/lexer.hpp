#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace lexer {

class Lexer {
public :

	TokenList tokens ; ///< output tokenlist
	PatternsMap patterns ; ///< input patterns

	Lexer () {}

	/*!
	 * \brief Lexer constructor
	 * \param pmap  Patterns Map to use for lexing
	 */
	Lexer (PatternsMap pmap) :
		patterns (pmap) {
		tokens = TokenList() ;
	}

	/*!
	 * \brief tokenizes a string into a token's list
	 * \param str     string to tokenize
	 * \param verbose set to true for verbose, false otherwise
	 * \param splits  set to true to tokenize by line, otherwise, tokenizes
	 * the whole file
	 */
	void tokenize (std::string str, bool verbose=false, bool splits=true) ;

private :

	/*!
	 * \brief tokenizeline tokenizes oneline at a time
	 * \param str  line to tokenize
	 */
	void tokenizeline (std::string str) ;
} ;// class Lexer

} // namespace lexer

} //namespace parselib

