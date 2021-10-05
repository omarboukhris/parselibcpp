#pragma once

#include <parselib/parsers/parsers.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree ;

namespace parselib {

using TreeList = std::vector<pt::ptree> ;
/*!
 * \brief eliminates the last char of a string if it's a dot (.)
 * \param name : node name to process
 */
std::string processnodename (std::string name) ;

class ParseSession {
public :
	myparsers::Grammar grammar ;
	myparsers::CYK parser ;
	lexer::Lexer tokenizer ;
	
	ParseSession () ;

	/*!
	 * \brief builds the instance by loading 
	 * the grammar from a text file
	 * \param filename : string path to file containing text to load
	 */
	void load_grammar (std::string filename, bool verbose=false) ;
	
	void store_json (std::string filename, std::string output_filename, bool verbose=false, size_t index=0);

	pt::ptree process2ptree (std::string filename, bool verbose=false, size_t index=0) ;

	/*!
	 * \brief parses source code in filename,
	 * unfolds the parse tree and optionnaly prints it
	 * \param filename : string path to file containing text to load
	 * \param verbose : bool
	 * True (by default) to print results, otherwise False
	 * \return Tree* processed parsetree if exists
	 */
	parsetree::Tree* processSource (std::string filename, bool verbose=false, size_t index=0) ;
	
private :

	/*!
	 * \brief unfolds parse tree in a factory generated dataformat
	 * \param code : parse tree => result from membership method
	 * \param parent : str => node's parent name
	 * \param verbose : bool true to talk
	 */
	parsetree::Tree* parse (parsetree::Tree* code=new parsetree::Tree(), std::string parent="") ;

	parsetree::AbsNode* processnode(parsetree::AbsNode::Token element);

	pt::ptree to_ptree (parsetree::AbsNode* tree) ;
	
} ; //class ParseSession
} ; //namespace parselib
