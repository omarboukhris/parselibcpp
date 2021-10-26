#pragma once

#include <parselib/utils/preprocessor.hpp>

#include <parselib/datastructure/grammar.h>

#include <parselib/utils/logger.h>


namespace parselib {

namespace parsers {

/*!
 * \brief The GenericGrammarParser class parses a tokenized grammar
 * into the appropriate  data format
 */
class GenericGrammarParser {

protected:

	utils::Preproc_ptr preproc ;
	utils::Logger_ptr logger;

public:
	/*!
	 * \brief GenericGrammarParser constructor
	 * \param preproc shared pointer of preprocessor object
	 */
	GenericGrammarParser (utils::Preproc_ptr preproc, utils::Logger_ptr logger) ;

	/*!
	 * \brief lex a grammar from textual form to tokenized
	 * \param txt_grammar : raw textual grammar source code filename
	 * \param verbose true to make it talk. false by default
	 */
	Grammar parse (std::string filename, bool verbose=false, bool splits=false) ;
} ;


} // parsers namespace 

} // parselib namespace

