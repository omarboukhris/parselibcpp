#pragma once

#include <parselib/datastructure/lexer.hpp>

namespace parselib {

namespace utils {

std::string transformtosource (lexer::Lexer::TokenList tokenizedgrammar) ;
std::string gettextfilecontent (std::string filename) ;

std::string cleanIfTerminal (std::string token) ;
std::string cleanRegex (std::string regx) ;
std::string escapeRegex (std::string regx) ;

typedef std::vector<std::string> StrList ; 
StrList split (std::string target, std::string delim) ;
std::string join (StrList strlist, std::string delim) ;

class Printer {
public :
	static void showinfo (std::string str) {
		std::cout << "(info) " << str << std::endl ;
	}
	
	static void showerr (std::string str) {
		std::cerr << "(error) " << str << std::endl ;
	}
} ;

class ArgvLex {
public :
	typedef std::vector<std::string> ArgList ;
	
	static ArgList compileargv (int argc, char** argv) {
		ArgList all_args;

		if (argc > 1) {
			all_args.assign(argv+1, argv + argc);
		}

		return all_args ;
	}
	
	ArgvLex (ArgList argv) ;
	
	std::string get (std::string key) ;

private :
	std::map <std::string, std::string> parsedargv ;
} ;

} //namespace utils

} //namespace parselib


