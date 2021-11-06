
#include <boost/variant.hpp>

#include <parselib/datastructure/operations/generalop.hpp>
#include <parselib/datastructure/operations/normop.hpp>

#include <parselib/utils/logger.h>

#include "parselibinstance.hpp"


namespace parselib {

ParseSession::ParseSession(int logLevel) {
	grammar   = Grammar() ;
	parser    = new parsers::CYK() ;
	tokenizer = lexer::Lexer () ;
	logger    = std::make_shared<utils::Logger>(logLevel) ;
}

ParseSession::~ParseSession() {
	delete parser;
}

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
	parsetree::Tree::TreePtr psrc (process_source(filename, verbose, index));
	std::cout << "in" << std::endl;
	std::cout << psrc->strUnfold() << std::endl;
	std::cout << "out" << std::endl;
	pt::ptree out = to_ptree(psrc) ;

	if (verbose) {
		std::stringstream ss;
		pt::json_parser::write_json(ss, out);
		std::cout << ss.str() << std::endl;
	}
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

parsetree::Tree::TreePtr ParseSession::process_source(std::string filename, bool verbose, size_t index) {

// 	StructFactory.readGrammar(self.grammar)
	parser = new parsers::CYK (grammar) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source, verbose) ;

	Frame result = parser->membership (tokenizer.tokens) ;
// 	Frame result = parser.parallel_membership (tokenizer.tokens) ;

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

			return parse (result[index]->unfold(), "") ; // something went horribly wrong here
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
			return nullptr;
		}
	}
}

parsetree::Tree::TreePtr ParseSession::parse(parsetree::Tree::TreePtr code, std::string parent) {
	using namespace std;
	//needs a do over
	parsetree::Tree* out = new parsetree::Tree() ;
	for (parsetree::Tree::Token& element : code->tokens) {

//		parsetree::Tree::Token out_element = parsetree::Tree::Token() ;
		if (element.first == "AXIOM") {

			cout << "axiom processing" << endl;
			return parse (element.second, "AXIOM") ;
		}
		
		element.first = processnodename(element.first) ;
		
		//part that handles labels changing (aliases)
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
//		std::cout << element.first << grammar.inKeeperKeys(element.first) << std::endl ;
		if (grammar.inKeeperKeys(element.first)) {

			parsetree::Tree* out_element;
			//std::cout << (grammar.isTokenSavable(parent, element.first)) << std::endl ; 

			if (grammar.keyIsStr(element.first)) {

				std::string out_elementstr = element.second->strUnfold () ;
				out_element = new parsetree::Tree(out_elementstr);
//				cout << "keystr " << out_element->getval() <<endl;
			}
			else if (grammar.isTokenSavable(parent, element.first)) {

				if (element.second->type == parsetree::Tree::NodeType::Branch) {

					out_element = new parsetree::Tree(parse(element.second, element.first)) ;
				}
				else { // terminal node
					out_element = new parsetree::Tree(element.second) ;
				}
			}
			else { //not savable, pass
				delete out_element;
				continue ;
			}

			// appending to result
			out->tokens.push_back(std::make_pair(element.first, std::make_shared<parsetree::Tree>(out_element)));
			delete out_element;


		}
	}
//	std::cout << std::make_shared<parsetree::Tree>(out) << std::endl ;
	return std::make_shared<parsetree::Tree>(out) ;
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
