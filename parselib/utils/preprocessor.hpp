#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <parselib/utils/io.hpp>

namespace parselib {

namespace utils {

class Preprocessor {
public :
	typedef std::vector<std::string> ImportQueue ;
	ImportQueue queue ;

	virtual lexer::Lexer::TokenList preprocess (std::string filename, lexer::Lexer::TokenList tokenlist) = 0 ;
	void addToQueue (std::string filename) {
		queue.push_back(filename) ;
	}
	
	void removeFromQueue (std::string filename) {
		// remove filename from queue
		ImportQueue newqueue = ImportQueue () ;
		
		for (std::string itr : queue) {
			if (itr != filename) {
				newqueue.push_back (itr);
			} 
		}
		queue = newqueue ;
	}
	
	bool queueIsEmpty () {
		return queue.size() == 0 ;
	}
} ;

class OnePassPreprocessor : public Preprocessor {

public :
	typedef std::vector<std::string> ProcessedFiles ;

	
	OnePassPreprocessor () {
		pwd = "" ;
		queue = ImportQueue () ;
		processed = ProcessedFiles () ; // to avoid nested imports
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
		if (isProcessed(filename)) {
			return lexer::Lexer::TokenList() ;
		}

		// get grammar directory 
		utils::StrList x = utils::split(filename, "/") ;
		x.pop_back() ;

		pwd = utils::join(x, "/") ; 

		lexer::Lexer::TokenList out_tokenlist = processimports (tokenlist) ;
		addToProcessed (filename) ;

		return out_tokenlist ;
	}

protected :
	std::string pwd ;
	
	ProcessedFiles processed ;

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
