
#include <boost/variant.hpp>

#include <parselib/operations/generalop.hpp>
#include <parselib/operations/normop.hpp>

#include <parselib/utils/logger.h>

#include "parsesession.hpp"


namespace parselib {

    // TODO : look for info method calls
ParseSession::ParseSession() {
	grammar   = Grammar() ;
	parser    = std::make_shared<parsers::CYK> (parsers::CYK()) ;
	tokenizer = lexer::Lexer () ;
    auto x = utils::ConsoleLogger("ParseSession");
	logger    = std::make_shared<utils::ConsoleLogger>(x) ;
}

/*!
 * \brief eliminates the last char of a string iTreePtrf it's a dot (.)
 * \param name : node name to process
 */
std::string processnodename(std::string name) {
	if (name.back() == '.') {
		name.pop_back() ;
	}
	return name ;
}

/*!
 * \brief builds the instance by loading
 * the grammar from a text file
 * \param filename : string path to file containing text to load
 */
void ParseSession::load_grammar(const std::string &filename, bool verbose) {
	utils::PreprocPtr preproc (new utils::OnePassPreprocessor()) ;
	parsers::GenericGrammarParser ggp (preproc, logger) ;

	Grammar grm = ggp.parse (filename, verbose, true) ;
	// grm.exportToFile(filename);
	this->grammar = normoperators::get2nf(grm) ;
	this->parser = std::make_shared<parsers::CYK> (parsers::CYK(this->grammar)) ;

}

/*!
 * \brief process_and_store_json processes input file into json output
 * \param filename        input filename
 * \param output_filename output json filename
 * \param verbose         self explanatory
 * \param index           frame index to use as a solution
 */
pt::ptree ParseSession::parse(const parsetree::TreePtr& code, const std::string& parent) {
    using Map = std::map<std::string, pt::ptree>;
    if (code->size() == 0) {
        return {};
    }

    Map map = Map();

    for (parsetree::Tree::Token &element: code->tokens()) {

        if (element.first == Token::Axiom) {
            return parse(element.second, Token::Axiom);
        }

        element.first = processnodename(element.first);

        //-----------------------------------------------
        // part that handles labels replacement (aliases)
        std::string node_label = element.first;
        if (grammar.inLabelsKeys(parent)) {
            auto &label_map = grammar.labels[parent];
            if (label_map.find(node_label) != label_map.end()) {
                node_label = label_map[node_label];
            }
        } //element.first is type, .second is val

        //-----------------------------------------------
        // check if element.first in keeper - storing in output data format : pt::ptree
        if (grammar.inKeeperKeys(node_label)) {
            //-------------------------------------------------
            // part handling str tokens
            if (grammar.keyIsStr(parent, element.first)) {
                std::string out_elementstr = element.second->strUnfold();
                pt::ptree tmp;
                tmp.put("", out_elementstr);
                map[node_label].push_back(std::make_pair("", tmp));
            }
            //-------------------------------------------------
            // part handling savable tokens as structs
            else if (grammar.isTokenSavable(parent, node_label)) {
                pt::ptree tmp;

                if (element.second->type() == parsetree::Tree::NodeType::Branch) {
                    tmp = parse(element.second, element.first);
                } else { // terminal node
                    tmp.put("", element.second->val());
                }
                map[node_label].push_back(std::make_pair("", tmp));
            }
        }
    }

    //-------------------------------------------------
    // save map in pt::ptree
    pt::ptree out = pt::ptree();
    for (const auto &item: map) {
        std::string key = item.first;
        pt::ptree tmp = item.second;
        out.add_child(key, tmp);
    }
    return out;
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

/*!
 * \brief parses source code in filename, unfolds the parse tree and optionnaly prints it
 * \param filename : string path to file containing text to load
 * \param verbose : bool
 * True (by default) to print results, otherwise False
 * \return boost ptree containing the parsed savable data or empty result if an error occured
 */
pt::ptree ParseSession::process_source_to_ptree(const std::string &filename, bool verbose, size_t index) {

    std::string source = utils::get_text_file_content(filename);

    tokenizer = lexer::Lexer(grammar.tokens);
    tokenizer.tokenize(source, verbose);

    Frame result = parser->membership(tokenizer.tokens);

    if (result.empty()) {
        if (verbose) {
            // x should point errors out if parsing failed
            logger->err("Empty result : no parse tree found, no error tracking possible");
        }
        return {};
    } else {
        index = (index > 0 && index < result.size()) ? index : 0;

        if (result[index]->nodetype == grammar.production_rules[Token::Axiom][0][0].value()) {

            // std::cout << "got axiom" << *result[index]->unfold().get() << std::endl ;
            pt::ptree output = parse(result[index]->unfold(), "");

            // show result if verbose
            if (verbose) {
                std::stringstream ss;
                pt::json_parser::write_json(ss, output);
                logger->info(ss.str());
            }
            return output;
        } else { // handle error
            std::fstream fstr(filename + ".log", std::fstream::out);

            if (fstr.is_open()) {
                logger->err("Parsing went wrong, check : " + filename + ".log");
                auto sus_tok = result.back();
                std::stringstream ss;
                ss << sus_tok->unfold();
                logger->err("Broken token seems to be <" + ss.str() + ">");
                fstr << result[index]->unfold();
                fstr.close();
            } else {
                logger->err("Could not open log file stream.");
            }
            return {};
        }
    }
}

/*!
 * \brief process_to_json processes input file into json string output
 * \param filename        input filename
 * \param verbose         self explanatory
 * \param index           frame index to use as a solution
 */
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
