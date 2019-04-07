
#include <parselib/datastructure/lexer.hpp>
#include <utils/io.hpp>

namespace parselib {

namespace lexer {

using namespace std ;

void Lexer::tokenize(string str, bool verbose) {
	tokens.clear () ;
	// split lines otherwise boost regex parser makes weird tokens
	for (std::string line : utils::split(str, "\n")) {
		tokenizeline(line);
	}
	if (!verbose) return ;
	for (Token tok : tokens) {
		cout << tok.second << "(" << tok.first << ")" << endl ;
	}
}


void Lexer::tokenizeline (string str) {
	
	// mix all patterns for group matching
	string regstr = "" ;
	for (Pattern pattern : patterns) { 
		regstr += "(?'" + pattern.second + "'" + pattern.first + ")|" ; 
	}
	
	boost::regex r(regstr);
	auto words_begin = boost::sregex_iterator( str.begin(), str.end(), r );
	auto words_end   = boost::sregex_iterator();
	
	MatchesMap matches ;

	for ( auto it = words_begin; it != words_end; ++it ) { //loop through matches
		boost::smatch match = *it ;
		for (auto pattern : patterns) {
			if (match[pattern.second].matched) { //find matched group
				matches[ it->position() ] = make_pair( it->str(), pattern.second);
// 				cout << it->position() << " " <<  it->str() << " " << pattern.second << endl ;
			}
		}
	}

	//write to result
	for ( auto & match : matches) {
		tokens.push_back(match.second) ;
	}
}

} // namespace lexer

} //namespace parselib