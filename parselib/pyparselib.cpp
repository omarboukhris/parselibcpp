
#include <parselib/parselibinstance.hpp>

#include <boost/python.hpp>

using namespace boost::python ;

namespace parselib {

extern "C" {
	
	ParseSession* new_session() { return new ParseSession() ; }
	void load_grammar(ParseSession* session, const char* filepath) { 
		session->load_grammar(std::string(filepath), false); 
	}
	void store_json(ParseSession* session, const char* filepath) { 
		session->store_json(std::string(filepath), std::string(filepath), false); 
	}
	void del_session(ParseSession* session) { delete session ; }
}

}



