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
//*
	class Token {
	public:

		Token()
			: m_token({"", ""})
		{}

		Token(std::string value, std::string type)
			: m_token({value, type})
		{}

		std::string &value(){
			return m_token.first;
		}

		std::string &key() {
			return m_token.second;
		}

		std::string &type() {
			return m_token.second;
		}

		friend bool operator == (const Token &t1, const Token t2) {
			return t1.m_token == t2.m_token;
		}
		friend bool operator != (const Token &t1, const Token t2) {
			return not (t1 == t2);
		}

	protected:
		// 1st:Value  | 2nd:Key(type)
		std::pair<std::string, std::string> m_token ;
	};
//*/
//	typedef std::pair<std::string, std::string> Token;

	typedef std::vector<Token> TokenList ;

	typedef Token Pattern ;
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
