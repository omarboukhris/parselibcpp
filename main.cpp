
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>
#include <utils/preprocessor.hpp>
#include <parsers/grammarparser.hpp>

using namespace std;
using namespace parselib ;

int main(int argc, char** argv){

	utils::ArgvLex::ArgList arglist = utils::ArgvLex::compileargv(argc, argv) ;

	utils::ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ? true : false ;
	utils::Printer::showinfo("verbose : " + argvlex.get("-v"));
	utils::Printer::showinfo("gsrc    : " + argvlex.get("--gsrc"));
	
	if (argvlex.get("--gsrc") != "False") {
		//parse argument
		string grammarfilename = argvlex.get("--gsrc") ;

		utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
		myparsers::GenericGrammarParser ggp (preproc) ;

		myparsers::Grammar grammar = ggp.parse (grammarfilename, verbose) ;
		cout << grammar ;
		
// 		CYK cyk = CYK(grammar) ;
// 		ParseTree tree = cyc.membership (text) ;
// 		tree.get(0).unfold () //or something
	}

}