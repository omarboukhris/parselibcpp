
#include <parselib/utils/io.hpp>

#include <bits/stdc++.h> 
#include <boost/algorithm/string.hpp>

#include <fstream>
#include <streambuf>
#include <string>

namespace parselib {

namespace utils {

using namespace std ;

/// \brief read a whole file (path @ string filename) in a string
string gettextfilecontent (string filename) {
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

ArgvLex::ArgvLex(ArgList argv) {
	parsedargv = {} ;
	for (string arg : argv) {
		StrList slist = split(arg, "=") ;

		switch (slist.size()) {
			case 1 :
				parsedargv[slist[0]] = "True" ;
				break ;
			case 2 :
				parsedargv[slist[0]] = slist[1] ;
				break ;
			default :
				//error fuckedup every thing
				//gets ignored... but shouldn't
				break ;
		} ;
	}
}

string ArgvLex::get(string key){
	if (parsedargv.find(key) != parsedargv.end()) {
		return parsedargv[key] ;
	}
	return "False" ;
}


string cleanRegex (string regx) {
	size_t newlen = regx.size()-4 ; //strip 2 caracters from the beginning + 2 from the end
	return regx.substr(2, newlen) ;
}

string cleanIfTerminal (string strtoken) {
	size_t pos = strtoken.find (".") ;
	
	if (pos == string::npos)
		return strtoken ;
	
	return strtoken.substr(0, pos) ;
}

string escapeRegex(string regx) {
	boost::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };

	string sanitized = boost::regex_replace( regx, specialChars, R"(\$&)" );
	return sanitized ;
}


StrList split (string target, string delim) {
    if (target.empty()) {
		//maybe show err : target is empty
		return StrList() ;
	}

    StrList v = StrList() ;
	boost::split(v, target, boost::is_any_of(delim)) ;
	return v;
}

string join (StrList strlist, string delim) {
	if (strlist.size() == 0) {
		return "" ;
	}
	string out = "" ;
// 	for (string s : strlist) {
	for (size_t i = 0 ; i < strlist.size()-1 ; i++) {
		std::string s = strlist[i] ;
		out += s + delim ;
	}
	return out+strlist.back() ;
}

std::string transformtosource ( TokenList tokenizedgrammar )
{
	string source = "" ;
	for (Token token : tokenizedgrammar) {
		source += token.type() + " " ;
	}
	return source ;
}


} //namespace utils

} //namespace parselib


