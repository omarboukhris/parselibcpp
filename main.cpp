
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>
#include <parselib/utils/preprocessor.hpp>
#include <parselib/parsers/grammarparser.hpp>
#include <parselib/parsers/cyk.hpp>

using namespace std;
using namespace parselib ;

int main(int argc, char** argv){

	utils::ArgvLex::ArgList arglist = utils::ArgvLex::compileargv(argc, argv) ;

	utils::ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ? true : false ;
	utils::Printer::showinfo("verbose : " + argvlex.get("-v"));
	utils::Printer::showinfo("gsrc    : " + argvlex.get("--gsrc"));
	utils::Printer::showinfo("src    : " + argvlex.get("--src"));
	
	if (argvlex.get("--gsrc") != "False") {
		//parse argument
		string grammarfilename = argvlex.get("--gsrc") ;

		utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
		myparsers::GenericGrammarParser ggp (preproc) ;

		myparsers::Grammar grammar = ggp.parse (grammarfilename, verbose) ;
// 		cout << grammar ;
		
		if (argvlex.get("--src") != "False") { // a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
			myparsers::CYK cyk (grammar) ;
			
			std::string source = utils::gettextfilecontent(sourcefilename) ;
			
			lexer::Lexer lex (grammar.tokens) ;
			lex.tokenize(source, verbose);
			
			// works perfectly fine
			myparsers::Frame result = cyk.membership(lex.tokens) ;
		}
	}

}