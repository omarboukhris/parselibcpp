#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <iostream>

// #include <regex>
#include <boost/regex.hpp>

#include <string>

#include <vector>
#include <map>
#include <iterator>


namespace parselib {
	// 1st:Value  | 2nd:Key(type)
	typedef std::pair<std::string, std::string> Token ;
	typedef std::vector<Token> TokenList ;

	typedef std::pair<std::string,std::string> Pattern ;
	typedef std::vector<Pattern> PatternsMap ;

	typedef std::map<size_t, Token> MatchesMap ;


	typedef TokenList Rule ;
	typedef std::vector<Rule> Rules ;
	typedef std::map <std::string, Rules> ProductionRules ;

	typedef std::vector<std::string> StrList ;
	typedef std::map<std::string, StrList> KeepingList ;

	typedef std::map<std::string, std::string> LabelReplacement ;
	typedef std::map<std::string, LabelReplacement> LabelReplacementMap ;
}
#endif // COMMON_TYPES_H
