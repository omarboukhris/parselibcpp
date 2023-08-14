#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <iostream>
#include <fstream>

// #include <regex>
#include <boost/regex.hpp>

#include <string>

#include <utility>
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
		Token() = default;

		Token(std::string  val, std::string  t)
            : value_(std::move(val)), type_(std::move(t))
        {}

		//
		// Accessors
		//

		inline std::string &value()
			{ return value_; }
		inline std::string &key()
			{ return type_; }
		inline std::string & type() { return type_; }

		//
		// File streams
		//

		/*!
		 * \brief write_to writes token to binary file (serialization)
		 * \param t_fstream open file stream
		 */
		void write_to (std::fstream &t_fstream) const {

			try {

				t_fstream.write(reinterpret_cast<char*>(value_.size()),
								sizeof(value_.size()));
				t_fstream.write(value_.c_str(), (long)value_.size());

				t_fstream.write(reinterpret_cast<char*>(type_.size()),
								sizeof(type_.size()));
				t_fstream.write(type_.c_str(), (long)type_.size());

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

				long valueSize, typeSize;

				t_fstream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
				char *value = new char[valueSize];
				t_fstream.read(value, valueSize);

				t_fstream.read(reinterpret_cast<char*>(&typeSize), sizeof(typeSize));
				char *type = new char[typeSize];
				t_fstream.read(type, typeSize);
				value_ = std::string(value);
                type_ = std::string(type_);

			} catch (std::exception &e) {

				std::cerr << "(token) caught exception while reading file: "
						  << e.what() << std::endl ;
			}
		}

		//
		// operators
		//

		friend bool operator == (const Token &t1, const Token& t2) {
			return (t1.value_ == t2.value_) and (t1.type_ == t2.type_);
		}
		friend bool operator != (const Token &t1, const Token& t2) {
			return not (t1 == t2);
		}

        friend bool operator < (const Token &t1, const Token& t2) {
            return t1.value_ < t2.value_;
        }


        friend std::ostream & operator<<(std::ostream &out, const Token &tok) {
            out << "(" << tok.value_ << ":" << tok.type_ << ")";
            return out;
        }

	protected:

		// 1st:Value  | 2nd:Key(type)
        std::string value_;
        std::string type_;

    public:
        static inline std::string NonTerminal = "NONTERMINAL";
        static inline std::string Terminal = "TERMINAL";
        static inline std::string Axiom = "AXIOM";
        static inline std::string Empty = "EMPTY";
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
