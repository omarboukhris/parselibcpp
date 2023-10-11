
#include <parselib/utils/io.hpp>
#include <parselib/operations/normop.hpp>
#include <parselib/parsesession.hpp>

namespace pt = boost::property_tree ;
namespace pl = parselib ;
using namespace pl::utils;

void showhelp () {
	std::cout <<
		"usage : parsexlib [arg] " << std::endl <<
		 std::endl <<
		 "  -h\t\t\t\t\t: help"  << std::endl <<
		std::endl <<
		"  --gsrc=path/to/grammar.grm \t\t: specifies grammar to use for parsing"  << std::endl <<
		std::endl <<
		"  use case 1 : process only 1 file"  << std::endl <<
		"    --src=path/to/source.something   : specifies source code to process"  << std::endl <<
		std::endl <<
		"  use case 2 : process recursively files"  << std::endl <<
		"    --dir=directory/to/glob/recurse  : directory to process"  << std::endl <<
		"    --ext=extension (ex : java, cpp) : globed files extension" << std::endl <<
		std::endl <<
	std::endl ;
}

int main(int argc, char** argv){

	auto arglist = ArgvLex::compileargv(argc, argv) ;

	ArgvLex argvlex (arglist) ;

	bool verbose = (argvlex.get("-v") == "True") ;
    GlobalConsoleLogger::info("verbose : " + argvlex.get("-v"));
    GlobalConsoleLogger::info("gsrc    : " + argvlex.get("--gsrc"));

	// change LogNone to LogBasic if needed
	pl::ParseSession parsesession ;
    //*
	if (argvlex.get("--gsrc") != "False") {
	//parse argument
		std::string grammarfilename = argvlex.get("--gsrc") ;
		parsesession.load_grammar(grammarfilename, verbose);

		// this part is mainly used for testing single files
		if (argvlex.get("--src") != "False") {

			// a source code have been provided
			std::string sourcefilename = argvlex.get("--src") ;
            GlobalConsoleLogger::info("now processing source code : " + sourcefilename);

			pt::ptree out = parsesession.process_source_to_ptree(sourcefilename, verbose);

            GlobalConsoleLogger::info("written json to : " + sourcefilename + ".json") ;
			pt::write_json(sourcefilename+".json", out) ;

		} else if (argvlex.get("--ext") != "False" && argvlex.get("--dir") != "False") {

			// glob recursively files with specified extention from directory
			// then parse
			FileGlober fileglober (argvlex.get("--dir"), argvlex.get("--ext")) ;

			for (const std::string &sourcefilename : fileglober.glob()) {
                GlobalConsoleLogger::info("now processing source code : " + sourcefilename);
                parsesession.process_and_store_json(sourcefilename, sourcefilename + ".json", verbose);
                GlobalConsoleLogger::info("written json to : " + sourcefilename + ".json") ;
			}
		}
	} else {
		showhelp();
	}
	return 0 ;
}


