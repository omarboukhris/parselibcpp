#include <parselib/parselibinstance.hpp>

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
	return __processResults(result, verbose) ;
}

parsetree::Tree* ParseSession::__processResults(myparsers::Frame x, bool verbose, size_t index) {
	if (x.size() == 0) {
		if (verbose) {
			// x should point errors out if parsing failed
			utils::Printer::showerr ("Empty result : no parse tree found") ; 
		}
	} else {
		if (verbose) {
			utils::Printer::showinfo ("Parsetree found") ;
		}
		index = (index >= 0 && index < x.size()) ? index : 0 ;
// 		std::cout << x[index]->unfold() << std::endl ;
		return __parse (x[index]->unfold(), "",  verbose) ;
	}
}

parsetree::Tree* ParseSession::__parse(parsetree::Tree* code, std::string parent, bool verbose) {

	parsetree::Tree out = parsetree::Tree() ;
	
	for (size_t i = 0 ; i < code->size() ; i++) {
// 		parsetree::Token element = code->at(i) ;
// 	
// 		parsetree::Token out_element = parsetree::Token() ;
// 		
// 		if (element.first == "AXIOM") {
// 			parsetree::Tree* newnode = new parsetree::Tree() ;
// 			newnode->push_back(element) ;
// 			return __parse (newnode, "AXIOM", verbose) ;
// 		}
// 		
// 		element.first = processnodename(element.first) ;
// 		
// 		//part that handles labels changing (aliases)
// 		for (auto item : grammar.labels) {
// 			std::string key = item.first ;
// 			if (parent == key) { //parent in labels.keys()
// 				
// 				for (auto subitem : grammar.labels[parent]) { 
// 					std::string subkey = subitem.first ;
// 					if (element.first == subkey) { //element type in label[parent]
// 						element.first = grammar.labels[parent][element.first] ;
// 						break ;
// 					}
// 				}
// 			} 
// 		}
// 
// 		if StructFactory.keyInFactory(parent, element.type) : #is savable
// 
// 			if StructFactory.keyIsStr(element.type): # node is str
// 				out_element = StructFactory.strUnfold (element.val)
// 			else :
// 				out_element = self.processnode (element, verbose)
// 			
// 			#appending to result
// 			if element.type in out.keys() :
// 				out[element.type].append(out_element)
// 			else :
// 				#out[element.type]=out_element
// 				out[element.type]=[out_element]
	}
	return new parsetree::Tree (out) ;
}

parsetree::Token ParseSession::processnode(parsetree::Token element, bool verbose) {
// 	# check if object in factory
// 	tmpClass = StructFactory.getStruct(element.type)
// 	# object is non terminal
// 	if tmpClass != None or type(element.val) == list:
// 		lst = self.__parse(
// 			code=element.val,
// 			parent=element.type,
// 			verbose=verbose
// 		)  # recurse
// 		return tmpClass(**lst)
// 
// 	else:  # terminal node
// 		return element.val
}



} //namespace parselib

