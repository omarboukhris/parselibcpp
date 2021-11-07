#pragma once

#include <parselib/parsers/parsers.hpp>
#include <parselib/utils/logger.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree ;

namespace parselib {

using TreeList = std::vector<pt::ptree> ;

using ParserPtr = std::shared_ptr<parsers::AbstractParser> ;

/*!
 * \brief eliminates the last char of a string iTreePtrf it's a dot (.)
 * \param name : node name to process
 */
std::string processnodename (std::string name) ;

class ParseSession {

public :

	ParseSession (int logLevel=utils::Logger::LogBasic) ;
	~ParseSession() ;

	/*!
	 * \brief builds the instance by loading 
	 * the grammar from a text file
	 * \param filename : string path to file containing text to load
	 */
	void load_grammar (std::string filename, bool verbose=false) ;
	
	/*!
	 * \brief store_json processes input file into json output
	 * \param filename        input filename
	 * \param output_filename output json filename
	 * \param verbose         self explanatory
	 * \param index           frame index to use as a solution
	 */
	void store_json (std::string filename, std::string output_filename, bool verbose=false, size_t index=0);

	/*!
	 * \brief process_to_json processes input file into json string output
	 * \param filename        input filename
	 * \param verbose         self explanatory
	 * \param index           frame index to use as a solution
	 */
	std::string process_to_json (std::string filename, bool verbose=false, size_t index=0);

	/*!
	 * \brief process2ptree processes input file into pt::ptree
	 * \param filename  input filen name
	 * \param verbose
	 * \param index     index of solution to use
	 * \return
	 */
	pt::ptree process2ptree (std::string filename, bool verbose=false, size_t index=0) ;

	/*!
	 * \brief parses source code in filename,
	 * unfolds the parse tree and optionnaly prints it
	 * \param filename : string path to file containing text to load
	 * \param verbose : bool
	 * True (by default) to print results, otherwise False
	 * \return Tree* processed parsetree if exists
	 */
	parsetree::Tree process_source (std::string filename, bool verbose=false, size_t index=0) ;
	
private :

	/*!
	 * \brief unfolds parse tree in a factory generated dataformat
	 * \param code : parse tree => result from membership method
	 * \param parent : str => node's parent name
	 * \param verbose : bool true to talk
	 */
	parsetree::Tree parse (parsetree::Tree::TreePtr code, std::string parent="") ;

	pt::ptree to_ptree (parsetree::Tree::TreePtr tree) ;

protected :

	Grammar grammar ;
	ParserPtr parser ;
	lexer::Lexer tokenizer ;
	utils::Logger_ptr logger ;
	
} ; //class ParseSession

} //namespace parselib
