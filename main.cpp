
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

		/*
		if (argvlex.get("--src") != "False") { 
		// a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
			parselib::utils::Printer::showinfo("now processing source code : " + sourcefilename);
			parselib::parsetree::Tree tree = parsesession.processSource(sourcefilename, verbose);
			if (argvlex.get("-vr") == "True") {
			// verbose result sample
			//	parselib::parsetree::Tree classname = tree["classes"];//["classname"] ;
			//	std::cout << classname ;
			}
		} else
		//*/ 
		if (
			argvlex.get("--ext") != "False" &&
			argvlex.get("--dir") != "False"
		) {
		// glob recursively files with specified extention from directory
		// then parse
			parselib::utils::FileGlober fileglober (argvlex.get("--dir"), argvlex.get("--ext")) ;
			parselib::utils::FileGlober::FilesList files = fileglober.glob() ;
			
			// parse all globed files
			parselib::parsetree::TreeList treelist = parselib::parsetree::TreeList () ;
			for (std::string sourcefilename : files) {
				parselib::utils::Printer::showinfo("now processing source code : " + sourcefilename);
				parselib::parsetree::Tree tree = parsesession.processSource(sourcefilename, verbose);				
				treelist.push_back(tree); //append tree to processed trees
			}
			// generate using processed trees
			for (parselib::parsetree::Tree tree : treelist) {
				// generator is caracterized by outputFileName and outputSourceCode
				// generate file name from something
				// callGenerator (tree) ; 
				// generator.process or something
			}
			
		}
	}
	
	return 0 ;
}


