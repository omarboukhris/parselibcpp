#pragma once
#include <parselib/parsers/parsers.hpp>

namespace parselib {
	
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
	void loadGrammar (std::string filename, bool verbose=false) ;

	/*!
	 * \brief parses source code in filename
	 * \param filename : string path to file containing text to load
	 */
	parsetree::Tree* processSource (std::string filename, bool verbose=false) ;
	
private :
	/*!
	 * \brief Unfolds the parse tree and optionnaly prints it
	 * \param x : UnitNode, TokenNode, BinNode from parselib.parsetree
	 * a list of the folded possible parse trees
	 * \param verbose : bool
	 * True (by default) to print results, otherwise False
	 */
	parsetree::Tree* processResults (myparsers::Frame x, bool verbose=false, size_t index=0) ;

	/*!
	 * \brief unfolds parse tree in a factory generated dataformat
	 * \param code : parse tree => result from membership method
	 * \param parent : str => node's parent name
	 * \param verbose : bool true to talk
	 */
	parsetree::Tree* parse (
		parsetree::Tree* code=new parsetree::Tree(), 
		std::string parent="", 
		bool verbose=false
	) ;

	parsetree::AbsNode::Token processnode(parsetree::AbsNode::Token element, bool verbose=false);

} ; //class ParseSession
} ; //namespace parselib
