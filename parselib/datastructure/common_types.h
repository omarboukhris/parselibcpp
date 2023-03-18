#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <iostream>
#include <fstream>

// #include <regex>
#include <boost/regex.hpp>

#include <string>

#include <vector>
#include <map>
#include <iterator>


namespace parselib {

	/*!
	 * \brief The Token class is the base class
	 * of the whole parselib framework
	 */
	class Token {
	public:

		/*!
		 * \brief Token default constructor
		 */
		Token()
			: m_token({"", ""})
		{}

		/*!
		 * \brief Token construction with key value pairing
		 * \param value
		 * \param type
		 */
		Token(std::string value, std::string type)
			: m_token({value, type})
		{}

		//
		// Accessors
		//

		std::string &value()
			{ return m_token.first; }
		std::string &key()
			{ return m_token.second; }
		std::string &type()
			{ return m_token.second; }

		//
		// File streams
		//

		/*!
		 * \brief write_to writes token to binary file (serialization)
		 * \param t_fstream open file stream
		 */
		void write_to (std::fstream &t_fstream) {

			try {

				t_fstream.write(reinterpret_cast<char*>(m_token.first.size()),
								sizeof(m_token.first.size()));
				t_fstream.write(m_token.first.c_str(), m_token.first.size());

				t_fstream.write(reinterpret_cast<char*>(m_token.second.size()),
								sizeof(m_token.second.size()));
				t_fstream.write(m_token.second.c_str(), m_token.second.size());

			} catch (std::exception &e) {

				std::cerr << "(token) caught exception while writing file: "
						  << e.what() << std::endl ;
			}
		}

		/*!
		 * \brief read_from reads serialized token from open file
		 * \param t_fstream  input open file stream
		 */
		void read_from (std::fstream &t_fstream) {

			try {

				size_t valueSize, typeSize;

				t_fstream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
				char *value = new char[valueSize];
				t_fstream.read(value, valueSize);

				t_fstream.read(reinterpret_cast<char*>(&typeSize), sizeof(typeSize));
				char *type = new char[typeSize];
				t_fstream.read(type, typeSize);
				m_token = std::make_pair(value, type) ;

			} catch (std::exception &e) {

				std::cerr << "(token) caught exception while reading file: "
						  << e.what() << std::endl ;
			}
		}

		//
		// operators
		//

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
