#pragma once

#include <parselib/utils/preprocessor.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/operations/naiveparsers.hpp>

namespace parselib {

using namespace operations ;

namespace myparsers {


class Grammar {

public :
	typedef std::map<std::string, SequentialParser::StrList> UnitRelation ;

	SequentialParser::ProductionRules production_rules ;
	
	SequentialParser::StrList strnodes ;
	SequentialParser::KeepingList keeper ;
	
	SequentialParser::LabelReplacementMap labels ;
	
	lexer::Lexer::TokenList tokens ;
	
	UnitRelation unitrelation ;

	Grammar () ;
	
	void merge (Grammar grammar) ;
	
	void exportToFile (std::string filename) ;

	/*!
	 * \brief parses a list of tokens in a grammar
	 * \param tokenizedgrammar : TokenList : list of tokens represented by the lexed grammar
	 * \param grammartokens : TokenList : list of tokens representing the lexed grammar
	 */
	void makegrammar (lexer::Lexer::TokenList tokenizedgrammar, lexer::Lexer::TokenList grammartokens) ;
	
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

class GenericGrammarParser {

public :
	
	utils::Preprocessor *preproc ;
	
	GenericGrammarParser (utils::Preprocessor *preproc) ;

	/*!
	 * \brief lex a grammar from textual form to tokenized
	 * \param txt_grammar : raw textual grammar source code filename
	 * \param verbose true to make it talk. false by default
	 */
	Grammar parse (std::string filename, bool verbose=false) ;
} ;


} // parsers namespace 

} // parselib namespace

