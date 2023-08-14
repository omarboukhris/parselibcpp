
#include <boost/algorithm/string/replace.hpp>

#include <parselib/datastructure/operations/generalop.hpp>
#include <parselib/parsers/naiveparsers.hpp>
#include <parselib/utils/io.hpp>

#include "grammar.h"

using namespace std ;

namespace parselib {

Grammar::Grammar () {
	production_rules = ProductionRules () ;
	labels = LabelReplacementMap () ;
	keeper = KeepingList () ;
	unitrelation = UnitRelation() ;
	strnodes = StrList () ;
	tokens = TokenList () ;
}

void Grammar::merge (const Grammar &grammar) {
	// unitrelation is computed later
	for (const Token &token : grammar.tokens) {
		tokens.push_back (token) ;
	}

	// loop through production_rules and merge
	for (const auto &item : grammar.production_rules) {

		//get key and rules
		std::string key = item.first ;
		Rules rules = item.second ;

		// if key if map then merge vectors
		if (production_rules.find(key) != production_rules.end() ) {
			for (const Rule &rule : rules) {
				production_rules[key].push_back(rule);
			}
		} else { //add new map entry
			production_rules[key] = rules ;
		}
	}

	//labels merge
	for (const auto &item : grammar.labels) {
		std::string key = item.first ;
		LabelReplacement mylabels = item.second ;

		if (labels.find(key) != labels.end()) {
			for (auto & sublab : mylabels) {
				std::string subkey = sublab.first ;
				labels[key][subkey] = sublab.second ;
			}
		} else {
			labels[key] = mylabels ;
		}
	}

	//keeper merge
	for (const auto &item : grammar.keeper) {
		std::string key = item.first ;
		StrList kept = item.second ;

		if (keeper.find(key) != keeper.end()) {
			for (const std::string &str : kept) {
				if (std::find(keeper[key].begin(), keeper[key].end(), str) == keeper[key].end()) {
					keeper[key].push_back(str);
				}
			}
		} else {
			keeper[key] = kept ;
		}

		// merge "all" entries
		for (const std::string & entry : kept) {
			//if entry not in keeper(all) add it, otherwise ignore it, already added
			if (std::find (keeper["all"].begin(), keeper["all"].end(), entry) == keeper["all"].end()) {
				keeper["all"].push_back(entry);
			}
		}
	}

	for (const std::string &s : grammar.strnodes) {
		strnodes.push_back(s) ;
	}
}

/*!
 * \brief parses a list of tokens in a grammar
 * \param tokenizedgrammar : TokenList : list of tokens represented by the lexed grammar
 * \param grammartokens : TokenList : list of tokens representing the lexed grammar
 */
void Grammar::makegrammar (const TokenList& tokenizedgrammar, const TokenList& grammartokens) {
	//ngp for naive grammar parser
	parsers::SequentialParser ngp = parsers::SequentialParser (tokenizedgrammar, grammartokens) ;

	ngp.parse () ;

	production_rules = ngp.production_rules ;
	tokens = ngp.tokens ;
	labels = ngp.labels ;
	strnodes = ngp.strnodes ;
	keeper = ngp.keeper ;

	*this = grammaroperators::eliminatedoubles (*this) ;
	//gramtest = checkproductionrules(self.production_rules) #is fuckedup
	//return gramtest
}

bool Grammar::inKeeperKeys(const string &toktype) {
	for (const auto &item : keeper) {
		if (item.first == toktype) {
			return true ;
		}
	}
	return std::find(keeper["all"].begin(), keeper["all"].end(), toktype) != keeper["all"].end() ;
}

bool Grammar::inLabelsKeys(const string &toktype) {
    return std::any_of(labels.begin(), labels.end(), [&](const auto &x) {
        return x.first == toktype;
    });
}

bool Grammar::keyIsStr(const string &toktype) {
    return std::any_of(strnodes.begin(), strnodes.end(), [&](const std::string &x) {
        return x == toktype;
    });
}

bool Grammar::isTokenSavable(const string &parent, const string &child) {
	if (parent == Token::Axiom || child == production_rules[Token::Axiom][0][0].value()) {
		return true ;
	}
	if (!inKeeperKeys(parent)) {
		return false ;
	}
	return std::find(keeper[parent].begin(), keeper[parent].end(), child) != keeper[parent].end() ;
}


/*!
 * \brief generates dot graph from a grammar and stores it in filename.png
 */
void Grammar::exportToFile(const string &filename) {
	std::string ss = "digraph {\n" ;
	for (const auto &item : production_rules) {
		std::string key = item.first ;
		boost::replace_all (key, "-", "");
		boost::replace_all (key, "/", "_");
		boost::replace_all (key, "[", "_");
		boost::replace_all (key, "]", "");
		boost::replace_all (key, ".", "_tok");
		Rules rules = item.second ;
		for (const Rule &rule : rules) {
			StrList r ;
			for (Token op : rule) {
				std::string val = op.value() ;
				boost::replace_all (val, "-", "");
				boost::replace_all (val, ".", "");
				boost::replace_all (val, "\'\'", "eps");
				boost::replace_all (val, "\"\"", "eps");
				boost::replace_all (val, "/", "_");
				boost::replace_all (val, "[", "_");
				boost::replace_all (val, "]", "");
				r.push_back(val) ;
			}
			ss += "\t" + key + " -> " ;
			ss += utils::join(r, "->") ;
			ss += " ;\n" ;
		}
	}
	ss += "}" ;
	std::ofstream filestream (filename + ".dot") ;
	if (!filestream.is_open ()) {
		utils::Printer::showerr("(exportToFile) Can't open file " + filename + ".dot");
		return ;
	}
	filestream << ss ;
	filestream.close() ;
	std::string cmd = "dot -Tpng -o " + filename + ".png " + filename + ".dot" ;
	std::system (cmd.c_str()) ;
	cmd = "rm " + filename + ".dot" ;
	std::system (cmd.c_str()) ;
}

/// \brief Screaming results for debug resons or verbose
string Grammar::getstr () {
	stringstream text_rule;

	for (const auto &item : production_rules) {
		string key = item.first ;
		Rules rules = item.second ;

		text_rule << "\nRULE " << key << " = [\n\t" ;

		StrList rule_in_a_line = StrList () ;

		for (const Rule &rule : rules) {
			StrList ruletxt = StrList () ;
			for (Token opr : rule) {
				ruletxt.push_back(opr.type()+"("+opr.value()+")");
			}
			string thisrule = utils::join(ruletxt, " ") ;
			rule_in_a_line.push_back(thisrule);
		}
		text_rule << utils::join(rule_in_a_line, "\n\t") << "\n]" ;
	}

	text_rule << "\n\n" << "LABELS = [\n" ;
	for (const auto &item : labels) {
		string key = item.first ;
		LabelReplacement labmap = item.second ;
		text_rule << key << " {\n" ;
		for (const auto &lab : labmap) {
			text_rule << "\t" << lab.first << " : " << lab.second << "\n" ;
		}
		text_rule << "}\n" ;
	}
	text_rule << "]\n" ;

	text_rule << "STRUCT = [\n" ;
	for (const auto &item : keeper) {
		string key = item.first ;
		StrList listkeep = item.second ;
		text_rule << "" << key << " {\n\t"
            << utils::join(listkeep, "\n\t")
            << "\n}\n" ;
	}
	text_rule << "]\n\n" ;

	text_rule << "STRNODE = [\n" << utils::join(strnodes, "") << "\n]\n\n" ;

	for (Token tok : tokens) {
		string label = tok.type() ;
		string regx = tok.value() ;
        text_rule << "TOKEN " << label << " = regex('" << regx << "')\n" ;
	}

	text_rule << "\nUNIT = [\n" ;
	for (const auto &item : unitrelation) {
		string key = item.first ;
		StrList listkeep = item.second ;
		text_rule << "" << key << " {\n\t"
            << utils::join(listkeep, "\n\t")
            << "\n}\n" ;
	}
	text_rule << "]\n\n" ;

	return text_rule.str() ;
}


}
