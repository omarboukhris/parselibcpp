
#include <parselib/operations/naiveparsers.hpp>
#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>

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
		//read file
		string grammarsource = utils::gettextfilecontent(grammarfilename) ;
		
		lexer::Lexer myLexer (
			operations::GenericGrammarTokenizer::grammartokens
		) ;
		operations::GenericGrammarTokenizer::tokenize (
			myLexer,
			grammarsource,
			verbose
		) ;

		/* add function to turn tokenlist to source for 2nd parsing pass
		lexer::Lexer gramLexer (
			operations::GenericGrammarTokenizer::genericgrammarprodrules
		) ;
		operations::GenericGrammarTokenizer::tokenize (
			gramLexer,
			grammarsource,
			verbose
		) ; //*/
		
	}

}