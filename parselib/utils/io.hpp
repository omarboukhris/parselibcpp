#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <boost/filesystem.hpp>

namespace parselib {

namespace utils {

std::string transformtosource (lexer::Lexer::TokenList tokenizedgrammar) ;
std::string gettextfilecontent (std::string filename) ;

std::string cleanIfTerminal (std::string token) ;
std::string cleanRegex (std::string regx) ;
std::string escapeRegex (std::string regx) ;

typedef std::vector<std::string> StrList ; 
StrList split (std::string target, std::string delim) ;
std::string join (StrList strlist, std::string delim) ;

class FileGlober {
public :
	typedef std::vector<std::string> FilesList ;
	FileGlober (const std::string folderpath, const std::string ext) {
		this->folderpath = folderpath ;
		this->ext = ext ;
	}
	FilesList glob () {
		FilesList files = FilesList() ;
		for(auto& p: boost::filesystem::recursive_directory_iterator(folderpath)) {
			std::string path = p.path().string() ;
			size_t startindex = path.length() - ext.length() ;
			if (path.substr(startindex-1) == std::string(".") + ext) {
				files.push_back(path);
			}
		}
		return files ;
	}
protected :
	std::string folderpath ;
	std::string ext ;
} ;

class Printer {
public :
	static void showinfo (std::string str) {
		std::cout << "(info) " << str << std::endl ;
	}
	
	static void showerr (std::string str) {
		std::cerr << "(error) " << str << std::endl ;
	}
} ;

class ProgressBar {
public :
	ProgressBar (size_t width, size_t max) {
		this->width = width ;
		this->max = max ;//(max/100.0)*width ;
		step = width*1.0/max ;
	}
	
	void update (size_t i) {
// 		std::cout << width/step << ":" << i << ":" << steps << std::endl ;
		std::cout << "[" ;
		float current_prog = 0.f ; //+step each loop
		size_t steps = current_prog*width/step ;
		for (size_t k = 0 ; k <= i ; ++k) {
			std::cout << "=" ;
		}
		std::cout << ">" ;
		for (size_t k = i ; k < max ; ++k) {
			std::cout << " " ;
		}
		std::cout << "] " << int(i*100.0f/max) << "% " ;
		if (i == width) {
			std::cout << "finished" ;
		} else {
			std::cout << "loading\r" ;
		}
	}

protected :
	float step ;
	size_t width ;
	size_t max ;
} ;

class ArgvLex {
public :
	typedef std::vector<std::string> ArgList ;
	
	static ArgList compileargv (int argc, char** argv) {
		ArgList all_args;

		if (argc > 1) {
			all_args.assign(argv+1, argv + argc);
		}

		return all_args ;
	}
	
	ArgvLex (ArgList argv) ;
	
	std::string get (std::string key) ;

private :
	std::map <std::string, std::string> parsedargv ;
} ;

} //namespace utils

} //namespace parselib


