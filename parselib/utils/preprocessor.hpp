#pragma once

#include <parselib/parsers/lexer.hpp>
#include <parselib/utils/io.hpp>

#include <unistd.h>

namespace parselib {

namespace utils {

class Preprocessor {
public :
	typedef std::vector<std::string> ImportQueue ;
	ImportQueue queue ;

	virtual TokenList preprocess (std::string filename, TokenList tokenlist) = 0 ;
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

typedef std::shared_ptr<utils::Preprocessor> Preproc_ptr;

class OnePassPreprocessor : public Preprocessor {

public :
	typedef std::vector<std::string> ProcessedFiles ;

	
	OnePassPreprocessor () {
		pwd = "" ;
		queue = ImportQueue () ;
		processed = ProcessedFiles () ; // to avoid nested imports
	}
	
	inline bool isProcessed (std::string filename) {
		ProcessedFiles::iterator itr = std::find(processed.begin(), processed.end(), filename) ;
		return itr != processed.end() ;
	}

	TokenList preprocess (std::string filename, TokenList tokenlist) {
		removeFromQueue (filename) ;
		if (isProcessed(filename)) {
			return TokenList() ;
		}

		// get grammar directory 
		utils::StrList x = utils::split(filename, "/") ;
		x.pop_back() ;

		pwd = utils::join(x, "/") ;

		TokenList out_tokenlist = processimports (tokenlist) ;
		processed.push_back (filename) ;

		return out_tokenlist ;
	}

protected :
	std::string pwd ;
	
	ProcessedFiles processed ;

	TokenList processimports (TokenList tokenlist) {
		TokenList outtok = TokenList () ;
		for (Token token : tokenlist) {
			if (token.type() == "IMPORT") {
				std::size_t quotepos = token.value().find("\"") ;
				std::string path = token.value().substr(quotepos+1) ;
				path.pop_back() ; //eliminate the last "
				std::string filename = pwd + "/" + path ;
//				std::cout << "IMPORT [" << filename << "]" << std::endl ;
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
