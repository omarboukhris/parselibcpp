
#include <utils/io.hpp>

#include "lexer.hpp"

namespace parselib {

namespace lexer {

using namespace std ;

void Lexer::tokenize(string str, bool verbose, bool splits) {
	tokens.clear () ;
	// split lines otherwise boost regex parser makes weird tokens
	if (splits) {
		for (std::string line : utils::split(str, "\n")) {
			tokenizeline(line);
		}
	} else {
		tokenizeline(str);
	}
	if (verbose) {
		for (Token tok : tokens) {
			cout << tok.type() << "(" << tok.value() << ")" << endl ;
		}
	}
}


void Lexer::tokenizeline (string str) {
	
	// mix all patterns for group matching
	string regstr = "" ;
	for (Pattern pattern : patterns) { 
		regstr += "(?'" + pattern.type() + "'" + pattern.value() + ")|" ;
	}
	
	boost::regex r(regstr);
	auto words_begin = boost::sregex_iterator( str.begin(), str.end(), r );
	auto words_end   = boost::sregex_iterator();
	
	MatchesMap matches ;

	for ( auto it = words_begin; it != words_end; ++it ) { //loop through matches
		boost::smatch match = *it ;
		for (auto pattern : patterns) {
			if (match[pattern.type()].matched) { //find matched group
				matches[ it->position() ] = Token( it->str(), pattern.type());
//				matches[ it->position() ] = make_pair( it->str(), pattern.type());
//				cout << it->position() << " " <<  it->str() << " " << pattern.value() << endl ;
			}
		}
	}
//	cout << "----------------" << endl ;
	//write to result
	for ( auto & match : matches) {
		tokens.push_back(match.second) ;
	}
}

} // namespace lexer

} //namespace parselib
