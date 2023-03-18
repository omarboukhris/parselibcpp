#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <parselib/datastructure/common_types.h>

namespace parselib {

/*!
 * \brief The Grammar class represents a grammar
 * in the context of language theory with some
 * useful internal mechanisms
 */
class Grammar {

public :

	typedef std::map<std::string, StrList> UnitRelation ;

	ProductionRules production_rules ;

	StrList strnodes ;
	KeepingList keeper ;

	LabelReplacementMap labels ;

	TokenList tokens ;

	UnitRelation unitrelation ;

	Grammar () ;

	/*!
	 * \brief merge input grammar with this (union)
	 * \param grammar input grammar to merge
	 */
	void merge (const Grammar &grammar) ;

	/*!
	 * \brief exportToFile exports grammar to dot file for graph generation
	 * \param filename path to exportation file
	 */
	void exportToFile (const std::string &filename) ;

	/*!
	 * \brief parses a list of tokens in a grammar
	 * \param tokenizedgrammar : TokenList : list of tokens represented by the lexed grammar
	 * \param grammartokens : TokenList : list of tokens representing the lexed grammar
	 */
	void makegrammar (TokenList tokenizedgrammar, TokenList grammartokens) ;

public:

	bool inKeeperKeys (const std::string &toktype) ;
	bool inLabelsKeys(const std::string &toktype) ;
	bool keyIsStr (const std::string &toktype) ;
	bool isTokenSavable(const std::string &parent, const std::string &child) ;

	friend std::ostream & operator<< (std::ostream & out, Grammar str) {
		out << str.getstr() ;
		return out ;
	}

private :
	std::string getstr () ;

} ; // end class Grammar

}

#endif // GRAMMAR_H
