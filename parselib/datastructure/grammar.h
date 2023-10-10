#pragma once

#include <parselib/datastructure/common_types.h>

namespace parselib {

/*!
 * \brief The Grammar class represents a grammar
 * in the context of language theory with some
 * useful internal mechanisms
 */
class Grammar: public GrammarStruct {

public :

	typedef std::map<std::string, StrList> UnitRelation ;

	UnitRelation unitrelation ;

	Grammar () = default;

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
	void makegrammar (const TokenList& tokenizedgrammar, const TokenList& grammartokens) ;

    friend std::ostream &operator<<(std::ostream &out, Grammar str) {
        out << str.getstr();
        return out;
    }

    bool isTokenList(const std::string &parent, const std::string &sub_node);

public:

	bool inKeeperKeys (const std::string &toktype) ;
	bool inLabelsKeys(const std::string &toktype) ;
	bool isTokenSavable(const std::string &parent, const std::string &child) ;
    bool keyIsStr(const std::string &parent, const std::string &toktype);

private :
	std::string getstr () override ;

} ; // end class Grammar

}
