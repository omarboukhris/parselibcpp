
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

void ParseSession::store_json(std::string filename, std::string output_filename, bool verbose, size_t index) {
	pt::ptree out = process_source_to_ptree(filename, verbose, index) ;
	pt::write_json(output_filename+".json", out) ;
}

std::string ParseSession::process_to_json (std::string filename, bool verbose, size_t index) {
	pt::ptree out = process_source_to_ptree(filename, verbose, index) ;
	std::stringstream ss;
	pt::write_json(ss, out);
	return ss.str();
}

pt::ptree ParseSession::process_source_to_ptree(std::string filename, bool verbose, size_t index) {

	parser = std::make_shared<parsers::CYK> (parsers::CYK(grammar)) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source, verbose) ;

	Frame result = parser->membership (tokenizer.tokens) ;

	if (result.size() == 0) {
		if (verbose) {
			// x should point errors out if parsing failed
			utils::Printer::showerr ("Empty result : no parse tree found, no error tracking possible") ;
		}
		return pt::ptree() ;
	} else {
		index = (index > 0 && index < result.size()) ? index : 0 ;

		if (result[index]->nodetype == grammar.production_rules["AXIOM"][0][0].value()) {

			// std::cout << "got axiom" << *result[index]->unfold().get() << std::endl ;
			pt::ptree output = parse (*result[index]->unfold().get(), "");

			// show result if verbose
			if (verbose) {
				std::stringstream ss;
				pt::json_parser::write_json(ss, output);
				utils::Printer::showinfo(ss.str());
			}
			return output ; // something goes horribly wrong here
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
			return pt::ptree();
		}
	}
	return pt::ptree();
}

pt::ptree ParseSession::parse(parsetree::Tree code, std::string parent) {
	using Map = std::map<std::string, pt::ptree> ;
	if (code.size() == 0) {
		return pt::ptree() ;
	}

	Map map = Map() ;

	for (parsetree::Tree::Token& element : code.tokens) {

		if (element.first == "AXIOM") {

			return parse (element.second, "AXIOM") ;
		}
		
		element.first = processnodename(element.first) ;

		//-----------------------------------------------
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


		//-----------------------------------------------
		// check if element.first in keeper - storing in output data format : pt::ptree
		if (grammar.inKeeperKeys(element.first)) {

			//-------------------------------------------------
			// part handling str tokens
			if (grammar.keyIsStr(element.first)) {
				std::string out_elementstr = element.second->strUnfold () ;
				pt::ptree tmp ;
				tmp.put ("", out_elementstr) ;
				map[element.first].push_back(std::make_pair("", tmp)) ;
			}
			//-------------------------------------------------
			// part handling savable tokens as structs
			else if (grammar.isTokenSavable(parent, element.first)) {

				if (element.second->type == parsetree::Tree::NodeType::Branch) {
					parsetree::Tree &param = *element.second.get();
					pt::ptree tmp = parse(param, element.first) ;
					map[element.first].push_back(std::make_pair("", tmp)) ;
				}
				else { // terminal node
					pt::ptree tmp  ;
					tmp.put ("", element.second->getval()) ;
					map[element.first].push_back(std::make_pair("", tmp)) ;
				}
			}
		}
	}

	//-------------------------------------------------
	// save map in pt::ptree
	pt::ptree out = pt::ptree() ;
	for (auto item : map) {
		std::string key = item.first ;
		pt::ptree tmp = item.second ;
		out.add_child(key, tmp) ;
	}
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
			pt::ptree tmp ;
			tmp.put ("", tok.second->getval()) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
		} else {
			pt::ptree tmp = to_ptree(tok.second) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
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
