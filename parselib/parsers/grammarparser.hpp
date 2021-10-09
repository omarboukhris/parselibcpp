#pragma once

#include <parselib/utils/preprocessor.hpp>

#include <parselib/datastructure/grammar.h>

namespace parselib {

namespace myparsers {

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

