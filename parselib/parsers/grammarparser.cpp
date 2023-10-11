
#include <fstream>

#include <parselib/operations/generalop.hpp>

#include <parselib/parsers/seqparsers.hpp>

#include <parselib/utils/io.hpp>

#include <parselib/utils/logger.h>

#include "grammarparser.hpp"


namespace parselib::parsers {


using namespace std ;
using namespace grammaroperators ;
using namespace utils ;

/*!
 * \brief GenericGrammarParser constructor
 * \param preproc shared pointer of preprocessor object
 * \param logger shared pointer of logger object
 */
GenericGrammarParser::GenericGrammarParser (PreprocPtr &preproc, LoggerPtr &logger) {
	//preprocessor class
	this->preproc = preproc ;
	this->logger  = logger  ;
}

/*!
 * \brief lex a grammar from textual form to tokenized
 * \param txt_grammar : raw textual grammar source code filename
 * \param verbose true to make it talk. false by default
 */
Grammar GenericGrammarParser::parse (std::string filename, bool verbose, bool splits) {
	Grammar out_grammar = Grammar() ;
	preproc->addToQueue (filename) ;

	while (!preproc->queueIsEmpty()) {

		filename = preproc->queue[0] ;

//		utils::GlobalConsoleLogger::info("now processing : " + filename);
        logger->info("now processing : " + filename);

		std::string source = utils::get_text_file_content (filename) ;

		//tokenize grammar source
		lexer::Lexer lang (GenericGrammarTokenizer::grammartokens) ;
		lang.tokenize(source, verbose, splits);

		//preprocessor here (one pass preprocessor)
		lang.tokens = preproc->preprocess (filename, lang.tokens) ;

		// text tokens are needed for next step
		std::string txtok = utils::transform_to_source (lang.tokens) ;
		
		// tokenize in abstract grammar tokens
		lexer::Lexer gram (GenericGrammarTokenizer::genericgrammarprodrules) ;
		
		gram.tokenize(txtok, verbose);

		// make production rules
		Grammar grammar = Grammar () ;

		grammar.makegrammar (gram.tokens, lang.tokens) ;

		out_grammar.merge (grammar) ;
	}
	if (verbose) {
		cout << out_grammar ;
	}
	return out_grammar ;
}

} // parselib namespace


