
#include <parselib/utils/io.hpp>
#include <parselib/parselibinstance.hpp>

// using namespace std;
// using namespace parselib ;
namespace pt = boost::property_tree ;

void showhelp () {
	std::cout <<
		"usage : parsexlib [arg] " << std::endl <<
		"\t--gsrc=path/to/grammar.grm \t\t: specifies grammar to use for parsing"  << std::endl <<
		"\tuse case 1 : process only 1 file"  << std::endl <<
		"\t\t--src=path/to/source.something  : specifies source code to process"  << std::endl <<
		"\tuse case 2 : process recursively files"  << std::endl <<
		"\t\t--dir=directory/to/glob/recurse : directory to process"  << std::endl <<
		"\t\t--ext=extension (ex : java, cpp): globed files extension" << std::endl <<
		"\t-h\t                                : help"  << std::endl <<
	std::endl ;
}

int main(int argc, char** argv){

	parselib::utils::ArgvLex::ArgList arglist = parselib::utils::ArgvLex::compileargv(argc, argv) ;

	parselib::utils::ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ? true : false ;
	parselib::utils::Printer::showinfo("verbose : " + argvlex.get("-v"));
	parselib::utils::Printer::showinfo("gsrc    : " + argvlex.get("--gsrc"));
	
	parselib::ParseSession parsesession = parselib::ParseSession() ;
	
	if (argvlex.get("-h") == "True") {
		showhelp () ;
	} else if (argvlex.get("--gsrc") != "False") {
	//parse argument
		std::string grammarfilename = argvlex.get("--gsrc") ;
		parsesession.loadGrammar(grammarfilename, verbose);

		//*
		// this part is mainly used for testing single files
		if (argvlex.get("--src") != "False") { 
		// a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
			parselib::utils::Printer::showinfo("now processing source code : " + sourcefilename);

			pt::ptree out = parsesession.process2ptree(sourcefilename, verbose);
			
			parselib::utils::Printer::showinfo("written json to : " + sourcefilename + ".json") ;
			pt::write_json(sourcefilename+".json", out) ;

		} else if (
			argvlex.get("--ext") != "False" &&
			argvlex.get("--dir") != "False" 
		) {
		// glob recursively files with specified extention from directory
		// then parse
			parselib::utils::FileGlober fileglober (argvlex.get("--dir"), argvlex.get("--ext")) ;
			
// 			parselib::TreeList treelist = parselib::TreeList() ;
			for (std::string sourcefilename : fileglober.glob()) {
				parselib::utils::Printer::showinfo("now processing source code : " + sourcefilename);
				pt::ptree out = parsesession.process2ptree(sourcefilename, verbose);
// 				treelist.push_back(out); //append tree to processed trees
				parselib::utils::Printer::showinfo("written json to : " + sourcefilename + ".json") ;
				pt::write_json(sourcefilename+".json", out) ;
			}
		}
	} else {
		showhelp();
	}
	
	return 0 ;
}


