#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>

namespace parselib {

namespace utils {

class OnePassPreprocessor {

public :
	typedef std::vector<std::string> ImportQueue ;
	typedef std::vector<std::string> ProcessedFiles ;

	std::string pwd ;
	
	ImportQueue queue ;
	ProcessedFiles processed ;
	
	OnePassPreprocessor () {
		pwd = "" ;
		queue = ImportQueue () ;
		processed = ProcessedFiles () ; // to avoid nested imports
	}
	
	void addToQueue (std::string filename) {
		queue.push_back(filename) ;
	}
	
	void removeFromQueue (std::string filename) {
		// remove filename from queue
		ImportQueue newqueue = ImportQueue () ;
		
// 		std::cout << "before : " << filename << std::endl ;
// 		for (auto i : queue)
// 			std::cout << i << " " ;
		
		for (std::string itr : queue) {
// 			std::cout << itr << std::endl ;
			if (itr != filename) {
				newqueue.push_back (itr);
			} 
		}
		queue = newqueue ;
// 		std::cout << "after : " << filename << std::endl ;
// 		for (auto i : queue)
// 			std::cout << i << " " ;
	}
	
	bool queueIsEmpty () {
		return queue.size() == 0 ;
	}
	
	bool isProcessed (std::string filename) {
		ProcessedFiles::iterator itr = std::find(processed.begin(), processed.end(), filename) ;
		return itr != processed.end() ;
	}

	void addToProcessed (std::string filename) {
		processed.push_back (filename) ;
	}

	lexer::Lexer::TokenList preprocess (std::string filename, lexer::Lexer::TokenList tokenlist) {
		removeFromQueue (filename) ;
// 		bug is up here !!!
		if (isProcessed(filename)) {
			return lexer::Lexer::TokenList() ;
		}

		// get grammar directory 
		utils::StrList x = utils::split(filename, "/") ; // x looks good, go check pwd, it's f*ed up
		x.pop_back() ;

		// looks good now (almost : adds a last /, maybe will cause bugs
		pwd = utils::join(x, "/") ; 

		lexer::Lexer::TokenList out_tokenlist = processimports (tokenlist) ;
		addToProcessed (filename) ;

		return out_tokenlist ;
	}

protected :

	lexer::Lexer::TokenList processimports (lexer::Lexer::TokenList tokenlist) {
		lexer::Lexer::TokenList outtok = lexer::Lexer::TokenList () ;
		for (lexer::Lexer::Token token : tokenlist) {
			if (token.second == "IMPORT") {
				std::size_t quotepos = token.first.find("\"") ;
				std::string path = token.first.substr(quotepos+1) ;
				path.pop_back() ; //eliminate the last "
				std::string filename = pwd + "/" + path ; 
// 				std::cout << filename << std::endl ;
				addToQueue (filename) ;
			} else {
				outtok.push_back (token) ;
			}
		}
		return outtok ;
	}
} ;

} // namespace preprocessor

} // namespace parselib
