#pragma once

#include <parselib/utils/preprocessor.hpp>

#include <parselib/datastructure/grammar.h>

namespace parselib {

namespace parsers {

class GenericGrammarParser {

protected:

	utils::Preproc_ptr preproc ;

public:
	/*!
	 * \brief GenericGrammarParser constructor
	 * \param preproc shared pointer of preprocessor object
	 */
	GenericGrammarParser (utils::Preproc_ptr preproc) ;

	/*!
	 * \brief lex a grammar from textual form to tokenized
	 * \param txt_grammar : raw textual grammar source code filename
	 * \param verbose true to make it talk. false by default
	 */
	Grammar parse (std::string filename, bool verbose=false) ;
} ;


} // parsers namespace 

} // parselib namespace

