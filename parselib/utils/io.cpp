
#include <parselib/utils/io.hpp>

#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <streambuf>
#include <string>

namespace parselib::utils {

using namespace std ;

/// \brief read a whole file (path @ string filename) in a string
string get_text_file_content (const string &filename) {
	ifstream filestream (filename) ;
	if (!filestream.is_open()) {
		utils::Printer::showerr("can't read file : " + filename);
		return "" ;
	}
	
	string out (
		(istreambuf_iterator<char>(filestream)),
		istreambuf_iterator<char>()
	) ;
	filestream.close();
	return out ;
}

ArgvLex::ArgvLex(const ArgList& argv) {
	parsedargv = {} ;
	for (const string& arg : argv) {
		StrList slist = split(arg, "=") ;

		switch (slist.size()) {
			case 1 :
				parsedargv[slist[0]] = "True" ;
				break ;
			case 2 :
				parsedargv[slist[0]] = slist[1] ;
				break ;
			default :
				// ignored
				break ;
		} ;
	}
}

string ArgvLex::get(const string& key){
	if (parsedargv.find(key) != parsedargv.end()) {
		return parsedargv[key] ;
	}
	return "False" ;
}


string clean_regex (const string& regx) {
	size_t newlen = regx.size()-4 ; //strip 2 caracters from the beginning + 2 from the end
	return regx.substr(2, newlen) ;
}

string clean_if_terminal (string strtoken) {
	size_t pos = strtoken.find ('.') ;
	
	if (pos == string::npos)
		return strtoken ;
	
	return strtoken.substr(0, pos) ;
}

string escape_regex(const string& regx) {
	boost::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };

	string sanitized = boost::regex_replace( regx, specialChars, R"(\$&)" );
	return sanitized ;
}


StrList split (string target, const string& delim) {
    if (target.empty()) {
		//maybe show err : target is empty
		return {} ;
	}

    StrList v = StrList() ;
	boost::split(v, target, boost::is_any_of(delim)) ;
	return v;
}

string join (StrList strlist, const string& delim) {
	if (strlist.empty()) {
		return "" ;
	}
	string out ;
	for (size_t i = 0 ; i < strlist.size()-1 ; i++) {
		std::string s = strlist[i] ;
		out += s + delim ;
	}
	return out+strlist.back() ;
}

std::string transform_to_source ( const TokenList& tokenizedgrammar )
{
	string source ;
	for (Token token : tokenizedgrammar) {
		source += token.type() + " " ;
	}
	return source ;
}


} //namespace parselib


