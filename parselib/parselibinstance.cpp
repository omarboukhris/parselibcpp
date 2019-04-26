#include <parselib/parselibinstance.hpp>
#include "operations/generalop.hpp"

namespace parselib {

ParseSession::ParseSession() {
	grammar   = myparsers::Grammar() ;
	parser    = myparsers::CYK() ;
	tokenizer = lexer::Lexer () ;
}

std::string processnodename(std::string name) {
	if (name.back() == '.') {
		name.pop_back() ;
	}
	return name ;
}

void ParseSession::loadGrammar(std::string filename, bool verbose) {
	utils::OnePassPreprocessor *preproc = new utils::OnePassPreprocessor() ;
	myparsers::GenericGrammarParser ggp (preproc) ;

	myparsers::Grammar grammar = ggp.parse (filename, verbose) ;
	
	this->grammar = grammar ;
}

parsetree::Tree* ParseSession::processSource(std::string filename, bool verbose) {

// 	StructFactory.readGrammar(self.grammar)
	parser = myparsers::CYK (grammar) ;
	std::string source = utils::gettextfilecontent(filename) ;
	
	tokenizer = lexer::Lexer(grammar.tokens) ;
	tokenizer.tokenize (source) ;

	myparsers::Frame result = parser.membership (tokenizer.tokens) ;
	return processResults(result, verbose) ;
}

parsetree::Tree* ParseSession::processResults(myparsers::Frame x, bool verbose, size_t index) {
	if (x.size() == 0) {
		if (verbose) {
			// x should point errors out if parsing failed
			utils::Printer::showerr ("Empty result : no parse tree found") ;
		}
		return nullptr ;
	} else {
		index = (index >= 0 && index < x.size()) ? index : 0 ;
		if (verbose) {
			utils::Printer::showinfo ("Parsetree found") ;
			std::cout << x[index]->unfold() << std::endl ;
		}
		return parse (x[index]->unfold(), "") ;
	}
}

parsetree::Tree* ParseSession::parse(parsetree::Tree* code, std::string parent) {
	//needs a do over
	parsetree::Tree* out = new parsetree::Tree() ;
// 	std::cout << code << std::endl ;
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

// 		check if element.first in keeper
		if (grammar.inKeeperKeys(element.first)) {
			parsetree::AbsNode* out_element = new parsetree::Tree();
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

} //namespace parselib
