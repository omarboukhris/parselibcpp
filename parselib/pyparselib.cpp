
#include <parselib/parsesession.hpp>

namespace parselib {

extern "C" {
	
	ParseSession* new_session(int logLevel) {
		switch (logLevel) {
			case 0:
			return new ParseSession(utils::LogLevel::LogNone) ;
			case 1:
			return new ParseSession(utils::LogLevel::LogBasic) ;
			default:
			return new ParseSession(utils::LogLevel::LogAll) ;
		}
	}

	void load_grammar(ParseSession* session, const char* filepath, bool verb) {
		session->load_grammar(std::string(filepath), verb);
	}

	void store_json(ParseSession* session, const char* filepath, bool verb) {
        session->process_and_store_json(std::string(filepath), std::string(filepath), verb);
	}

	const char* get_json(ParseSession* session, const char* filepath, bool verb) {
		static std::string ss ;
		ss = session->process_to_json(std::string(filepath), verb);
		return ss.c_str();
	}

	void del_session(ParseSession* session) { delete session ; }
}

}



