
#include <parselib/utils/io.hpp>
#include <parselib/parselibinstance.hpp>

// using namespace std;
// using namespace parselib ;

int main(int argc, char** argv){

	parselib::utils::ArgvLex::ArgList arglist = parselib::utils::ArgvLex::compileargv(argc, argv) ;

	parselib::utils::ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ? true : false ;
	parselib::utils::Printer::showinfo("verbose : " + argvlex.get("-v"));
	parselib::utils::Printer::showinfo("gsrc    : " + argvlex.get("--gsrc"));
	parselib::utils::Printer::showinfo("src     : " + argvlex.get("--src"));
	
	parselib::ParseSession parsesession = parselib::ParseSession() ;
	
	if (argvlex.get("--gsrc") != "False") {
	//parse argument
		std::string grammarfilename = argvlex.get("--gsrc") ;
		parsesession.loadGrammar(grammarfilename, verbose);

		if (argvlex.get("--src") != "False") { 
		// a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
			parselib::parsetree::Tree* tree = parsesession.processSource(sourcefilename, verbose);
			if (verbose) {
// 				parselib::parsetree::Tree* classname = tree->at("classes")->at("classname") ;
				parselib::parsetree::Tree* classname = (*(*tree)["classes"])["classname"] ;
				std::cout << classname << std::endl	 ;
			}
		}
	}
	
	return 0 ;
}


