#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

namespace lexer {

class Lexer {
public :

	/// output tokenlist
	TokenList tokens ;

	/// input patterns
	PatternsMap patterns ;
	
	Lexer () {}
	Lexer (PatternsMap pmap) : 
		patterns (pmap) {
		tokens = TokenList() ;
	}

	void tokenize (std::string str, bool verbose=false) ;
private :
	void tokenizeline (std::string str) ;
} ;// class Lexer

} // namespace lexer

} //namespace parselib

