
#include <boost/variant.hpp>

#include <parselib/datastructure/operations/generalop.hpp>
#include <parselib/datastructure/operations/normop.hpp>

#include <parselib/utils/logger.h>

#include "parselibinstance.hpp"


namespace parselib {

ParseSession::ParseSession(int logLevel) {
	grammar   = Grammar() ;
	parser    = std::make_shared<parsers::CYK> (parsers::CYK()) ;
	tokenizer = lexer::Lexer () ;
	logger    = std::make_shared<utils::Logger>(logLevel) ;
}

ParseSession::~ParseSession()
{}

std::string processnodename(std::string name) {
	if (name.back() == '.') {
		name.pop_back() ;
	}
	return name ;
}

void ParseSession::load_grammar(std::string filename, bool verbose) {
	utils::Preproc_ptr preproc (new utils::OnePassPreprocessor()) ;
	parsers::GenericGrammarParser ggp (preproc, logger) ;

	Grammar grammar = ggp.parse (filename, verbose, true) ;
	// grammar.exportToFile(filename);
	this->grammar = normoperators::get2nf(grammar) ;
}

pt::ptree ParseSession::process2ptree(std::string filename, bool verbose, size_t index) {
	parsetree::Tree psrc = process_source(filename, verbose, index);
//	std::cout << "in" << std::endl;
	std::cout << psrc.strUnfold() << std::endl;
//	std::cout << "out" << std::endl;
	pt::ptree out ;//= to_ptree(psrc) ;

//	if (verbose) {
//		std::stringstream ss;
//		pt::json_parser::write_json(ss, out);
//		std::cout << ss.str() << std::endl;
//	}
	return out ;
}

void ParseSession::store_json(std::string filename, std::string output_filename, bool verbose, size_t index) {
	pt::ptree out = process2ptree(filename, verbose, index) ;
	pt::write_json(output_filename+".json", out) ;
}

std::string ParseSession::process_to_json (std::string filename, bool verbose, size_t index) {
	pt::ptree out = process2ptree(filename, verbose, index) ;
	std::stringstream ss;
	pt::write_json(ss, out);
	return ss.str();
}

parsetree::Tree ParseSession::process_source(std::string filename, bool verbose, size_t index) {

// 	StructFactory.readGrammar(self.grammar)
	parser = std::make_shared<parsers::CYK> (parsers::CYK(grammar)) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source, verbose) ;

	Frame result = parser->membership (tokenizer.tokens) ;

	if (result.size() == 0) {
		if (verbose) {
			// x should point errors out if parsing failed
			utils::Printer::showerr ("Empty result : no parse tree found") ;
		}
		return nullptr ;
	} else {
		index = (index > 0 && index < result.size()) ? index : 0 ;

		if (result[index]->nodetype == grammar.production_rules["AXIOM"][0][0].value()) {

			std::cout << "got axiom" << result[index]->unfold() << std::endl ;

			return parse (result[index]->unfold(), "") ; // something goes horribly wrong here
		}
		else { // handle error
			std::fstream fstr (filename + ".log", std::fstream::out);

			if (fstr.is_open()) {
				utils::Printer::showerr("Parsing went wrong, check : " + filename + ".log");
				auto sus_tok = result.back();
				std::stringstream ss ;
				ss << sus_tok->unfold() ;
				utils::Printer::showerr("Broken token seems to be <" + ss.str() + ">") ;
				fstr << result[index]->unfold();
				fstr.close();
			}
			else {
				utils::Printer::showerr("Could not open log file stream.");
			}
			return parsetree::Tree();
		}
	}
	return parsetree::Tree();
}

/*!
TODO:  THIS NEEDS TO BE REBUILT FROM THE GROUND UP
 */
parsetree::Tree ParseSession::parse(parsetree::Tree::TreePtr code, std::string parent) {
	using namespace std;
	//needs a do over
	parsetree::Tree out ;

	for (parsetree::Tree::Token& element : code->tokens) {

//		parsetree::Tree::Token out_element = parsetree::Tree::Token() ;
		if (element.first == "AXIOM") {

			return parse (element.second, "AXIOM") ;
		}
		
		element.first = processnodename(element.first) ;

		//-----------------------------------------------
		// ok
		// part that handles labels replacement (aliases)
		if (grammar.inLabelsKeys(parent)) {

			for (auto subitem : grammar.labels[parent]) {

				std::string subkey = subitem.first ;

				if (element.first == subkey) { //element type in label[parent]

					element.first = grammar.labels[parent][element.first] ;
					break ;
				}
			}
		} //element.first is type, .second is val

		// check if element.first in keeper
		 std::cout << element.first << " " << grammar.inKeeperKeys(element.first) << std::endl ;
		if (grammar.inKeeperKeys(element.first)) {

			parsetree::Tree out_element;

			if (grammar.keyIsStr(element.first)) {
				std::string out_elementstr = element.second->strUnfold () ;
				out_element = parsetree::Tree(out_elementstr);
				std::cout << "keyIsStr : " << out_element.strUnfold() << std::endl ;
			}
			else if (grammar.isTokenSavable(parent, element.first)) {

				if (element.second->type == parsetree::Tree::NodeType::Branch) {

					out_element = parse(element.second, element.first) ;
					std::cout << "token save branch : " << out_element.strUnfold() << std::endl ;
				}
				else { // terminal node
					std::cout << "elmt : " << element.second << std::endl;
					out_element = parsetree::Tree(element.second) ;
					std::cout << "terminal save : " << out_element.getval() << std::endl ;
				}
			}
			else { //not savable, pass
				continue ;
			}

			// appending to result
			out.tokens.push_back(
				std::make_pair(
					element.first,
					std::make_shared<parsetree::Tree>(
						&out_element)));

		}
	}
//	std::cout << std::make_shared<parsetree::Tree>(out) << std::endl ;
//	return std::make_shared<parsetree::Tree>(out) ;
	return out ;
}

pt::ptree ParseSession::to_ptree(parsetree::Tree::TreePtr tree) {
	using Map = std::map<std::string, pt::ptree> ;
	if (tree == nullptr) {
		return pt::ptree() ;
	}

	// use map to correctly parse into ptree
	// something is fucked up otherwise in json
	Map map = Map() ; 
	for (auto& tok : tree->tokens) {
		if (tok.second->type == parsetree::Tree::NodeType::Leaf) {
			std::cout << tok.first << ":" << tok.second->strUnfold() << std::endl ;
			pt::ptree tmp ;
			tmp.put ("", tok.second->getval()) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
		} else {
//			pt::ptree tmp = to_ptree(tok.second) ;
//			map[tok.first].push_back(std::make_pair("", tmp)) ;
		}
	}

	pt::ptree out = pt::ptree() ;
	for (auto item : map) {
		std::string key = item.first ;
		pt::ptree tmp = item.second ;
		out.add_child(key, tmp) ;
	}
	return out ;
}



} //namespace parselib
