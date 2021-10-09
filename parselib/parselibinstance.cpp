
#include <boost/variant.hpp>

#include <parselib/datastructure/operations/generalop.hpp>
#include <parselib/datastructure/operations/normop.hpp>

#include "parselibinstance.hpp"

namespace parselib {

ParseSession::ParseSession() {
	grammar   = Grammar() ;
	parser    = new parsers::CYK() ;
	tokenizer = lexer::Lexer () ;
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
	std::shared_ptr<utils::OnePassPreprocessor> preproc (new utils::OnePassPreprocessor()) ;
	parsers::GenericGrammarParser ggp (preproc) ;

	Grammar grammar = ggp.parse (filename, verbose) ;
	// grammar.exportToFile(filename);
	this->grammar = normoperators::get2nf(grammar) ;
}

pt::ptree ParseSession::process2ptree(std::string filename, bool verbose, size_t index) {
	pt::ptree out = to_ptree(processSource(filename, false, index)) ;
	
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

parsetree::Tree* ParseSession::processSource(std::string filename, bool verbose, size_t index) {

// 	StructFactory.readGrammar(self.grammar)
	parser = new parsers::CYK (grammar) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source) ;

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
		if (verbose) {
			utils::Printer::showinfo ("Parsetree found") ;
			std::cout << result[index]->unfold() << std::endl ;
		}
		return parse (result[index]->unfold(), "") ;
	}
}

parsetree::Tree* ParseSession::parse(parsetree::Tree* code, std::string parent) {
	//needs a do over
	parsetree::Tree* out = new parsetree::Tree() ;
// 	std::cout << code << ":;" << std::endl ;
	for (parsetree::AbsNode::Token element : code->tokens) {

		parsetree::AbsNode::Token out_element = parsetree::AbsNode::Token() ;
		if (element.first == "AXIOM") {
			return parse (new parsetree::Tree(element.second), "AXIOM") ;
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
		//std::cout << element.first << grammar.inKeeperKeys(element.first) << std::endl ;
		if (grammar.inKeeperKeys(element.first)) {
			parsetree::AbsNode* out_element = new parsetree::Tree();
			//std::cout << (grammar.isTokenSavable(parent, element.first)) << std::endl ; 
			if (grammar.keyIsStr(element.first)) {
				std::string out_elementstr = element.second->strUnfold () ;
				out_element = new parsetree::Leaf(out_elementstr) ;
			} else if (grammar.isTokenSavable(parent, element.first)) {
				out_element = processnode (element) ;
			} else { //not savable, pass
				delete out_element ;
				continue ;
			}

			// appending to result
			out->tokens.push_back(
				std::make_pair(element.first, out_element));
		}
	}
	//std::cout << out << std::endl ;
	return out ;
}

parsetree::AbsNode* ParseSession::processnode(parsetree::AbsNode::Token element) {
// 	for each comp in element => copy element.second in result if Leaf
// 								call parse() if com is Tree
	if (element.second->type == "tree") {
		return parse(
			new parsetree::Tree(element.second),
			element.first
		) ;
	} else { // terminal node
		return element.second ;
	}
}

pt::ptree ParseSession::to_ptree(parsetree::AbsNode *tree) {
	using Map = std::map<std::string, pt::ptree> ;
	if (tree == nullptr) {
		return pt::ptree() ;
	}

	// use map to correctly parse into ptree
	// something is fucked up otherwise in json
	Map map = Map() ; 
	for (parsetree::AbsNode::Token tok : tree->tokens) {
		if (tok.second->type == "leaf") {
			pt::ptree tmp, tmplist ;
			tmp.put ("", tok.second->getval()) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
		} else {
			pt::ptree tmp = to_ptree(tok.second), tmplist ;
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
