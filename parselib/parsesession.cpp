
#include <boost/variant.hpp>

#include <parselib/datastructure/operations/generalop.hpp>
#include <parselib/datastructure/operations/normop.hpp>

#include <parselib/utils/logger.h>

#include "parsesession.hpp"


namespace parselib {

ParseSession::ParseSession(utils::LogLevel logLevel) {
	grammar   = Grammar() ;
	parser    = std::make_shared<parsers::CYK> (parsers::CYK()) ;
	tokenizer = lexer::Lexer () ;
	logger    = std::make_shared<utils::Logger>(logLevel) ;
}

std::string processnodename(std::string name) {
	if (name.back() == '.') {
		name.pop_back() ;
	}
	return name ;
}

void ParseSession::load_grammar(const std::string &filename, bool verbose) {
	utils::PreprocPtr preproc (new utils::OnePassPreprocessor()) ;
	parsers::GenericGrammarParser ggp (preproc, logger) ;

	Grammar grm = ggp.parse (filename, verbose, true) ;
	// grm.exportToFile(filename);
	this->grammar = normoperators::get2nf(grm) ;
}

pt::ptree ParseSession::parse(const parsetree::TreePtr& code, const std::string& parent) {
	using Map = std::map<std::string, pt::ptree> ;
	if (code->size() == 0) {
		return {} ;
	}

	Map map = Map() ;

	for (parsetree::Tree::Token& element : code->tokens()) {

		if (element.first == "AXIOM") {

			return parse (element.second, "AXIOM") ;
		}
		
		element.first = processnodename(element.first) ;

		//-----------------------------------------------
		// part that handles labels replacement (aliases)
		if (grammar.inLabelsKeys(parent)) {

			for (const auto &subitem : grammar.labels[parent]) {

				std::string subkey = subitem.first ;

				if (element.first == subkey) { //element type in label[parent]

					element.first = grammar.labels[parent][element.first] ;
					break ;
				}
			}
		} //element.first is type, .second is val


		//-----------------------------------------------
		// check if element.first in keeper - storing in output data format : pt::ptree
		if (grammar.inKeeperKeys(element.first)) {

			//-------------------------------------------------
			// part handling str tokens
			if (grammar.keyIsStr(element.first)) {
				std::string out_elementstr = element.second->strUnfold () ;
				pt::ptree tmp ;
				tmp.put ("", out_elementstr) ;
				map[element.first].push_back(std::make_pair("", tmp)) ;
			}
			//-------------------------------------------------
			// part handling savable tokens as structs
			else if (grammar.isTokenSavable(parent, element.first)) {

				if (element.second->type() == parsetree::Tree::NodeType::Branch) {
					parsetree::TreePtr &param = element.second;
					pt::ptree tmp = parse(param, element.first) ;
					map[element.first].push_back(std::make_pair("", tmp)) ;
				}
				else { // terminal node
					pt::ptree tmp  ;
					tmp.put ("", element.second->val()) ;
					map[element.first].push_back(std::make_pair("", tmp)) ;
				}
			}
		}
	}

	//-------------------------------------------------
	// save map in pt::ptree
	pt::ptree out = pt::ptree() ;
	for (const auto &item : map) {
		std::string key = item.first ;
		pt::ptree tmp = item.second ;
		out.add_child(key, tmp) ;
	}
	return out ;
}

pt::ptree ParseSession::to_ptree(const parsetree::TreePtr& tree) {
	using Map = std::map<std::string, pt::ptree> ;
	if (tree == nullptr) {
		return {} ;
	}

	// use map to correctly parse into ptree
	// something is fucked up otherwise in json
	Map map;
	for (auto& tok : tree->tokens()) {
		if (tok.second->type() == parsetree::Tree::NodeType::Leaf) {
			pt::ptree tmp ;
			tmp.put ("", tok.second->val()) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
		} else {
			pt::ptree tmp = to_ptree(tok.second) ;
			map[tok.first].push_back(std::make_pair("", tmp)) ;
		}
	}

	pt::ptree out;
	for (const auto& item : map) {
		std::string key = item.first ;
		pt::ptree tmp = item.second ;
		out.add_child(key, tmp) ;
	}
	return out ;
}

pt::ptree ParseSession::process_source_to_ptree(const std::string &filename, bool verbose, size_t index) {

    std::string source = utils::get_text_file_content(filename);

    tokenizer = lexer::Lexer(grammar.tokens);
    tokenizer.tokenize(source, verbose);

    Frame result = parser->membership(tokenizer.tokens);

    if (result.empty()) {
        if (verbose) {
            // x should point errors out if parsing failed
            utils::Printer::showerr("Empty result : no parse tree found, no error tracking possible");
        }
        return {};
    } else {
        index = (index > 0 && index < result.size()) ? index : 0;

        if (result[index]->nodetype == grammar.production_rules["AXIOM"][0][0].value()) {

            // std::cout << "got axiom" << *result[index]->unfold().get() << std::endl ;
            pt::ptree output = parse(result[index]->unfold(), "");

            // show result if verbose
            if (verbose) {
                std::stringstream ss;
                pt::json_parser::write_json(ss, output);
                utils::Printer::showinfo(ss.str());
            }
            return output;
        } else { // handle error
            std::fstream fstr(filename + ".log", std::fstream::out);

            if (fstr.is_open()) {
                utils::Printer::showerr("Parsing went wrong, check : " + filename + ".log");
                auto sus_tok = result.back();
                std::stringstream ss;
                ss << sus_tok->unfold();
                utils::Printer::showerr("Broken token seems to be <" + ss.str() + ">");
                fstr << result[index]->unfold();
                fstr.close();
            } else {
                utils::Printer::showerr("Could not open log file stream.");
            }
            return {};
        }
    }
}

std::string ParseSession::process_to_json(const std::string &filename, bool verbose, size_t index) {
    pt::ptree out = process_source_to_ptree(filename, verbose, index) ;
    std::stringstream ss;
    pt::write_json(ss, out);
    return ss.str();
}

void ParseSession::process_and_store_json(const std::string &filename, const std::string &output_filename, bool verbose, size_t index) {
    pt::ptree out = process_source_to_ptree(filename, verbose, index) ;
    pt::write_json(output_filename+".json", out) ;
}

} //namespace parselib
