#pragma once

#include <parselib/parsers/lexer.hpp>
#include <parselib/utils/io.hpp>

#include <unistd.h>

namespace parselib::utils {

class Preprocessor {
public :
	typedef std::vector<std::string> ImportQueue ;
	ImportQueue queue ;

	virtual TokenList preprocess (std::string filename, TokenList tokenlist) = 0 ;
	void addToQueue (const std::string& filename) {
		queue.push_back(filename) ;
	}

	void removeFromQueue (const std::string& filename) {
		// remove filename from queue
		ImportQueue newqueue = ImportQueue () ;
		
		for (const std::string& itr : queue) {
			if (itr != filename) {
				newqueue.push_back (itr);
			} 
		}
		queue = newqueue ;
	}
	
	[[nodiscard]] bool queueIsEmpty () const {
		return queue.empty() ;
	}
} ;

typedef std::shared_ptr<utils::Preprocessor> PreprocPtr;

class OnePassPreprocessor : public Preprocessor {

public :
	typedef std::vector<std::string> ProcessedFiles ;

	
	OnePassPreprocessor () {
		pwd = "" ;
		queue = ImportQueue () ;
		processed = ProcessedFiles () ; // to avoid nested imports
	}
	
	inline bool isProcessed (const std::string& filename) {
		auto itr = std::find(processed.begin(), processed.end(), filename) ;
		return itr != processed.end() ;
	}

	TokenList preprocess (std::string filename, TokenList tokenlist) override {
		removeFromQueue (filename) ;
		if (isProcessed(filename)) {
			return {} ;
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

	TokenList processimports (const TokenList& tokenlist) {
		TokenList outtok = TokenList () ;
		for (Token token : tokenlist) {
			if (token.type() == "IMPORT") {
				std::size_t quotepos = token.value().find('\"') ;
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

} // namespace parselib
