#pragma once

#include <parselib/datastructure/common_types.h>
#include <boost/filesystem.hpp>

namespace parselib {

namespace utils {

std::string transformtosource (TokenList tokenizedgrammar) ;
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
		std::cout << "[" ;
		size_t k = 0 ;
		while (k++ < size_t(i*1.f*width/max)) {
			std::cout << "=" ;
		}
		std::cout << ">" ;
		while (k++ < width) {
			std::cout << " " ;
		}
		std::cout << "] " << int(i*100.0f/max) << "% " ;
		if (i == max-1) {
			std::cout << "\r[" ;
			for (size_t k = 0 ; k++ < width-1;) {
				std::cout << "=" ;
			}
			std::cout << ">] 100%     finished" ;
		} else {
			std::cout << "] " << int(i*100.0f/max) << "% " ;
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


