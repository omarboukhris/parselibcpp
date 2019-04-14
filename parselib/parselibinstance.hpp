#pragma once
#include <parselib/parsers/parsers.hpp>

namespace parselib {
	
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

	void processSource (std::string filename, bool verbose=false) ;
	
	/*!
	 * \brief Unfolds the parse tree and optionnaly prints it
	 * \param x : UnitNode, TokenNode, BinNode from parselib.parsetree
	 * a list of the folded possible parse trees
	 * \param verbose : bool
	 * True (by default) to print results, otherwise False
	 */
	void __processResults (myparsers::Frame x, bool verbose=false) ;

	static std::string __processnodename (std::string name) {
		if (name.back() == '.') {
			name.pop_back() ;
		}
		return name ;
	}

// def __parse (self, code=[], parent="", verbose=False) :
// 	"""unfolds parse tree in a factory generated dataformat
// 	
// 		Parameters
// 		----------
// 		code : parse tree
// 			result from membership method
// 		
// 		parent : str 
// 			node's parent name
// 		
// 		verbose : bool
// 			True to talk
// 	"""
// 	i = 0
// 	out = odict()
// 	while i < len(code) :
// 		element = code[i]
// 		out_element = None
// 		
// 		if element.type == "AXIOM" :
// 			return self.__parse (element.val, "AXIOM", verbose)
// 		
// 		element.type = ParselibInstance.__processnodename(element.type)
// 		
// 		#part that handles labels changing (aliases)
// 		if parent in self.grammar.labels.keys() :
// 			if element.type in self.grammar.labels[parent].keys() :
// 				element.type = self.grammar.labels[parent][element.type]
// 
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
// 		i += 1
// 	#print(out)
// 
// 	return out
// 
// def processnode(self, element, verbose):
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

} ; //class ParseSession
} ; //namespace parselib
