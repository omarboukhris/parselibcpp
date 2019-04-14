#include <parselib/parselibinstance.hpp>

namespace parselib {

ParseSession::ParseSession() {
	grammar   = myparsers::Grammar() ;
	parser    = myparsers::CYK() ;
	tokenizer = lexer::Lexer () ;
}

void ParseSession::loadGrammar(std::string filename, bool verbose) {
	utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
	myparsers::GenericGrammarParser ggp (preproc) ;

	myparsers::Grammar grammar = ggp.parse (filename, verbose) ;
	
	this->grammar = grammar ;
}

void ParseSession::processSource(std::string filename, bool verbose) {

// 	StructFactory.readGrammar(self.grammar)
	parser = myparsers::CYK (grammar) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source) ;

	myparsers::Frame result = parser.membership (tokenizer.tokens) ;
 	return __processResults(result, verbose) ;
}

void ParseSession::__processResults(myparsers::Frame x, bool verbose) {
	if (x.size() == 0) {
		if (verbose) {
			// x should point errors out if parsing failed
			utils::Printer::showerr ("Empty result : no parse tree found") ; 
		}
	} else {
		if (verbose) {
			utils::Printer::showinfo ("Parsetree found") ;
			std::cout << x.size() ;
		}
// 		return __parse (x[0].unfold(), verbose) ;
	}
}

} //namespace parselib

