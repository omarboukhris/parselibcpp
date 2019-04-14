
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>
#include <parselib/utils/preprocessor.hpp>
#include <parselib/parsers/grammarparser.hpp>
#include <parselib/parsers/cyk.hpp>

#include <parselib/parselibinstance.hpp>

using namespace std;
// using namespace parselib ;

int main(int argc, char** argv){

	parselib::utils::ArgvLex::ArgList arglist = parselib::utils::ArgvLex::compileargv(argc, argv) ;

	parselib::utils::ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ? true : false ;
	parselib::utils::Printer::showinfo("verbose : " + argvlex.get("-v"));
	parselib::utils::Printer::showinfo("gsrc    : " + argvlex.get("--gsrc"));
	parselib::utils::Printer::showinfo("src    : " + argvlex.get("--src"));
	
	parselib::ParseSession parsesession = parselib::ParseSession() ;
	
	if (argvlex.get("--gsrc") != "False") {
	//parse argument
		string grammarfilename = argvlex.get("--gsrc") ;
		parsesession.loadGrammar(grammarfilename, verbose);

// 		parselib::utils::OnePassPreprocessor *preproc = new parselib::utils::OnePassPreprocessor() ;
// 		parselib::myparsers::GenericGrammarParser ggp (preproc) ;
// 
// 		parselib::myparsers::Grammar grammar = ggp.parse (grammarfilename, verbose) ;
		
		
		if (argvlex.get("--src") != "False") { 
		// a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
			parsesession.processSource(sourcefilename, verbose);

// 			parselib::myparsers::CYK cyk (grammar) ;
// 			
// 			std::string source = parselib::utils::gettextfilecontent(sourcefilename) ;
// 			
// 			parselib::lexer::Lexer lex (grammar.tokens) ;
// 			lex.tokenize(source, verbose);
// 			
// 			// works perfectly fine
// 			parselib::myparsers::Frame result = cyk.membership(lex.tokens) ;
// 			cout << result.size() ;
		}
	}

}