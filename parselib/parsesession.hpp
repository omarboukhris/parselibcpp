#pragma once

#include <parselib/parsers/parsers.hpp>
#include <parselib/utils/logger.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree ;

namespace parselib {

using TreeList = std::vector<pt::ptree> ;

using ParserPtr = std::shared_ptr<parsers::AbstractParser> ;

/// \brief eliminates the last char of a string iTreePtrf it's a dot (.)
std::string processnodename (std::string name) ;

class ParseSession {
public :

	explicit ParseSession () ;
	~ParseSession() = default;

    /// \brief builds the instance by loading
	void load_grammar (const std::string &filename, bool verbose=false) ;

    /// \brief process_and_store_json processes input file into json output
	void process_and_store_json (const std::string &filename, const std::string& output_filename, bool verbose=false, size_t index=0);

    /// \brief process_to_json processes input file into json string output
    std::string process_to_json (const std::string &filename, bool verbose=false, size_t index=0);

    /// \brief parses source code in filename, unfolds the parse tree and optionnaly prints it
    pt::ptree process_source_to_ptree (const std::string& filename, bool verbose=false, size_t index=0) ;


private :

    /// \brief unfolds parse tree in a factory generated dataformat
	pt::ptree parse (const parsetree::TreePtr& code, const std::string& parent="") ;

	pt::ptree to_ptree (const parsetree::TreePtr& tree) ;

protected :

	Grammar grammar ;
	ParserPtr parser ;
	lexer::Lexer tokenizer ;
	utils::LoggerPtr logger ;
} ; //class ParseSession

} //namespace parselib
