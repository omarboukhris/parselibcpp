#pragma once

#include <parselib/datastructure/common_types.h>
#include <boost/filesystem.hpp>

namespace parselib::utils {

/*!
 * \brief transform_to_source transforms tokenized grammar back to
 * string data
 * \param tokenizedgrammar
 * \return std::string containing the source
 */
std::string transform_to_source (const TokenList& tokenizedgrammar) ;
/*!
 * \brief get_text_file_content read text content from filename
 * \param filename
 * \return content of the file
 */
std::string get_text_file_content (const std::string &filename) ;

/*!
 * \brief clean_if_terminal, cleans up token before processing
 * \param token
 */
std::string clean_if_terminal (std::string token) ;
std::string clean_regex (const std::string& regx) ;
std::string escape_regex (const std::string& regx) ;

StrVect split (std::string target, const std::string& delim) ;
std::string join (StrVect strlist, const std::string& delim) ;

typedef std::vector<std::string> FilesList ;

/*!
 * \brief The FileGlober class recursively list files
 * in the specified directory with the right extension
 */
class FileGlober {
public :

	FileGlober (const std::string& folderpath, const std::string& ext) {
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
	static void showinfo (const std::string& str) {
		std::cout << "(info) " << str << std::endl ;
	}

	static void showerr (const std::string& str) {
		std::cerr << "(error) " << str << std::endl ;
	}
} ;

class ProgressBar {
public :
	explicit ProgressBar (size_t max) {
		this->max = max ;
	}
	
	void update (size_t i) const {
		std::cout << "\r" << int(i*100.0f/max) << "%" << std::flush;
		if (i == max-1) {
			std::cout << "\r100%" ;
		}
	}

protected :
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
	
	explicit ArgvLex (const ArgList& argv) ;
	
	std::string get (const std::string& key) ;

private :
	std::map <std::string, std::string> parsedargv ;
} ;

} //namespace parselib


