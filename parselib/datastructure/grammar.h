#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <parselib/datastructure/common_types.h>

namespace parselib {

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

	void merge (Grammar grammar) ;

	void exportToFile (std::string filename) ;

	/*!
	 * \brief parses a list of tokens in a grammar
	 * \param tokenizedgrammar : TokenList : list of tokens represented by the lexed grammar
	 * \param grammartokens : TokenList : list of tokens representing the lexed grammar
	 */
	void makegrammar (TokenList tokenizedgrammar, TokenList grammartokens) ;

	bool inKeeperKeys (std::string toktype) ;
	bool inLabelsKeys(std::string toktype) ;
	bool keyIsStr (std::string toktype) ;
	bool isTokenSavable(std::string parent, std::string child) ;

	friend std::ostream & operator<< (std::ostream & out, Grammar str) {
		out << str.getstr() ;
		return out ;
	}

private :
	std::string getstr () ;

} ; // end class Grammar

}

#endif // GRAMMAR_H
