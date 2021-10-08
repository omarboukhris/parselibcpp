#pragma once

#include <iterator>
#include <iostream>
#include <string>
// #include <regex>
#include <boost/regex.hpp>
#include <list>
#include <map>

namespace parselib {

// 1st:Value  | 2nd:Key(type)
typedef std::pair<std::string, std::string> Token ;
typedef std::vector<Token> TokenList ;

typedef std::pair<std::string,std::string> Pattern ;
typedef std::vector<Pattern> PatternsMap ;

typedef std::map<size_t, Token> MatchesMap ;


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

