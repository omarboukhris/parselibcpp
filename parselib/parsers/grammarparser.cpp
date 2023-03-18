
#include <fstream>

#include <parselib/datastructure/operations/generalop.hpp>

#include <parselib/parsers/naiveparsers.hpp>

#include <parselib/utils/io.hpp>

#include <parselib/utils/logger.h>

#include "grammarparser.hpp"


namespace parselib {

namespace parsers {


using namespace std ;
using namespace grammaroperators ;
using namespace utils ;


GenericGrammarParser::GenericGrammarParser (PreprocPtr &preproc, LoggerPtr &logger) {
	//preprocessor class
	this->preproc = preproc ;
	this->logger  = logger  ;
}

Grammar GenericGrammarParser::parse (std::string filename, bool verbose, bool splits) {
	Grammar out_grammar = Grammar() ;
	preproc->addToQueue (filename) ;

	while (!preproc->queueIsEmpty()) {

		filename = preproc->queue[0] ;

//		utils::Printer::showinfo("now processing : " + filename);
		logger->log_basic_info("now processing : " + filename);

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

} // parsers namespace 

} // parselib namespace


