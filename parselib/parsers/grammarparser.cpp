#include <parselib/parsers/grammarparser.hpp>

#include <parselib/utils/io.hpp>
#include <parselib/operations/generalop.hpp>
#include <parselib/operations/normop.hpp>

using namespace std ;

namespace parselib {

using namespace operations ;

namespace myparsers {


Grammar::Grammar () {
	production_rules = SequentialParser::ProductionRules () ;
	labels = SequentialParser::LabelReplacementMap () ;
	keeper = SequentialParser::KeepingList () ;
	unitrelation = UnitRelation() ; 
	strnodes = SequentialParser::StrList () ;
	tokens = lexer::Lexer::TokenList () ;
}
	
void Grammar::merge (Grammar grammar) {
	// unitrelation is computed later
	for (lexer::Lexer::Token token : grammar.tokens) {
		tokens.push_back (token) ;
	}

	// loop through production_rules and merge
	for (auto item : grammar.production_rules) {
	
		//get key and rules
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		
		// if key if map then merge vectors
		if (production_rules.find(key) != production_rules.end() ) {
			for (SequentialParser::Rule rule : rules) {
				production_rules[key].push_back(rule);
			}
		} else { //add new map entry
			production_rules[key] = rules ;
		}
	}
	
	//labels merge
	for (auto item : grammar.labels) {
		std::string key = item.first ;
		SequentialParser::LabelReplacement mylabels = item.second ;
	
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
	for (auto item : grammar.keeper) {
		std::string key = item.first ;
		SequentialParser::StrList kept = item.second ;
		
		if (keeper.find(key) != keeper.end()) {
			for (std::string str : kept) {
				if (std::find(keeper[key].begin(), keeper[key].end(), str) == keeper[key].end()) {
					keeper[key].push_back(str);
				}
			}
		} else {
			keeper[key] = kept ;
		}
		
		// merge "all" entries
		for (std::string entry : kept) {
			//if entry not in keeper(all) add it, otherwise ignore it, already added
			if (std::find (keeper["all"].begin(), keeper["all"].end(), entry) == keeper["all"].end()) {
				keeper["all"].push_back(entry);
			}
		}
	}

	for (std::string s : grammar.strnodes) {
		strnodes.push_back(s) ;
	}
}

/*!
 * \brief parses a list of tokens in a grammar
 * \param tokenizedgrammar : TokenList : list of tokens represented by the lexed grammar
 * \param grammartokens : TokenList : list of tokens representing the lexed grammar
 */
void Grammar::makegrammar (lexer::Lexer::TokenList tokenizedgrammar, lexer::Lexer::TokenList grammartokens) {
	//ngp for naive grammar parser
	SequentialParser ngp = SequentialParser (tokenizedgrammar, grammartokens) ;

	ngp.parse () ;

	production_rules = ngp.production_rules ;
	tokens = ngp.tokens ;
	labels = ngp.labels ;
	strnodes = ngp.strnodes ;
	keeper = ngp.keeper ;

	*this = grammaroperators::eliminatedoubles (*this) ;
	//gramtest = checkproductionrules(self.production_rules) #is fuckedup
	//return gramtest
	return ;
}

bool Grammar::inKeeperKeys(string toktype) {
	for (auto item : keeper) {
		if (item.first == toktype) {
			return true ;
		}
	}
	return std::find(keeper["all"].begin(), keeper["all"].end(), toktype) != keeper["all"].end() ;
}

bool Grammar::inLabelsKeys(string toktype) {
	for (auto item : labels) {
		string key = item.first ;
		if (toktype == key) { //element type in keeper.keys()
			return true ;			
		}
	}
	return false ;
}

bool Grammar::keyIsStr(string toktype) {
	for (string token : strnodes) {
		if (token == toktype) {
			return true ;
		}
	}
	return false ;
}

bool Grammar::isTokenSavable(string parent, string child) {
	if (parent == "AXIOM" || 
		child == production_rules["AXIOM"][0][0].first
	) {
		return true ;
	}
	if (!inKeeperKeys(parent)) {
		return false ;
	}
	return std::find(keeper[parent].begin(), keeper[parent].end(), child) != keeper[parent].end() ;
}


// 	void saveGraph (self, filename) {
// 		"""generates dot graph from a grammar and stores it in filename.png
// 		this should be updated .. and moved
// 		"""
// 		ss = "digraph {\n"
// 		for key, rules in self.production_rules.items() :
// 			for rule in rules :
// 				r = [op.val for op in rule]
// 				r = [i.replace ("-", "") for i in r]
// 				r = [i.replace (".", "") for i in r]
// 				r = [i.replace ("\'\'", "eps") for i in r]
// 				r = [i.replace ("\"\"", "eps") for i in r]
// 				r = [i.replace ("/", "_") for i in r]
// 				k = key.replace ("-", "")
// 				k = k.replace ("/", "_")
// 				k = k.replace (".", "_tok")
// 				ss += "\t" + k + " -> " 
// 				ss += " -> ".join (r)
// 				ss += " ;\n"
// 		ss += "}"
// 		filestream = open (filename + '.dot', 'w') 
// 		filestream.write(ss)
// 		filestream.close ()
// 		cmd = 'dot -Tpng -o ' + filename + '.png ' + filename + '.dot'
// 		os.system (cmd)
// 		cmd = 'rm ' + filename + '.dot'
// 		os.system (cmd)
// 	}

// 	def save (self, filename) :
// 		"""save parsed grammar in pickle file"""
// 		serialFile = open (filename, "wb")
// 		pickle.dump (self.production_rules, serialFile)
// 		pickle.dump (self.unitrelation, serialFile)
// 		pickle.dump (self.labels, serialFile)
// 		pickle.dump (self.keeper, serialFile)
// 		pickle.dump (self.strnodes, serialFile)
// 		pickle.dump (self.tokens, serialFile)
// 		serialFile.close()
// 	def load (self, filename) :
// 		"""load grammar from pickle file"""
// 		serialFile = open (filename, "rb")
// 		self.production_rules = pickle.load (serialFile)
// 		self.unitrelation = pickle.load (serialFile)
// 		self.labels = pickle.load (serialFile)
// 		self.keeper = pickle.load (serialFile)
// 		self.strnodes = pickle.load(serialFile)
// 		self.tokens = pickle.load (serialFile)
// 		serialFile.close()
// 

/// \brief Screaming results for debug resons or verbose
string Grammar::getstr () {
	string text_rule = "" ;

	for (auto item : production_rules) {
		string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		
		text_rule += "\nRULE " + key + " = [\n\t" ;
		
		SequentialParser::StrList rule_in_a_line = SequentialParser::StrList () ;
		
		for (SequentialParser::Rule rule : rules) {
			SequentialParser::StrList ruletxt = SequentialParser::StrList () ;
			for (lexer::Lexer::Token opr : rule) {
				ruletxt.push_back(opr.second+"("+opr.first+")");
			}
			string thisrule = utils::join(ruletxt, " ") ;
			rule_in_a_line.push_back(thisrule);
		}
		text_rule += utils::join(rule_in_a_line, "\n\t") + "]" ;		
	}
	
	text_rule += "\n\n" ;
	
	text_rule += "LABELS = [\n" ;
	for (auto item : labels) {
		string key = item.first ;
		SequentialParser::LabelReplacement labmap = item.second ;
		text_rule += key + " {\n" ;
		for (auto lab : labmap) {
			text_rule += "\t" + lab.first + " : " + lab.second + "\n" ;
		}
		text_rule += "}\n" ;
	}
	text_rule += "]\n" ;

	text_rule += "STRUCT = [\n" ;
	for (auto item : keeper) {
		string key = item.first ;
		SequentialParser::StrList listkeep = item.second ;
		text_rule += "" + key + " {\n\t" ;
		text_rule += utils::join(listkeep, "\n\t") ;
		text_rule += "}\n" ;
	}
	text_rule += "\n]\n\n" ;
	
	text_rule += "STRNODE = [\n" + utils::join(strnodes, "") + "\n]\n\n" ;

	for (lexer::Lexer::Token tok : tokens) {
		string label = tok.second ;
		string regx = tok.first ;
		text_rule += "TOKEN " + label + " = regex('" + regx + "')\n" ;
	}

	text_rule += "UNIT = [\n" ;
	for (auto item : unitrelation) {
		string key = item.first ;
		SequentialParser::StrList listkeep = item.second ;
		text_rule += "" + key + " {\n\t" ;
		text_rule += utils::join(listkeep, "\n\t") ;
		text_rule += "}\n" ;
	}
	text_rule += "\n]\n\n" ;
	
	return text_rule ;
}

GenericGrammarParser::GenericGrammarParser (utils::Preprocessor *preproc) {
	//preprocessor class
	this->preproc = preproc ;
}

/*!
 * \brief lex a grammar from textual form to tokenized
 * \param txt_grammar : raw textual grammar source code filename
 * \param verbose true to make it talk. false by default
 */
Grammar GenericGrammarParser::parse (std::string filename, bool verbose) {
	Grammar out_grammar = Grammar() ;
	preproc->addToQueue (filename) ;

	while (!preproc->queueIsEmpty()) {

		filename = preproc->queue[0] ;
		utils::Printer::showinfo("now processing : " + filename);
		std::string source = utils::gettextfilecontent (filename) ;
		
		//tokenize grammar source
		lexer::Lexer lang (operations::GenericGrammarTokenizer::grammartokens) ;
		lang = operations::GenericGrammarTokenizer::tokenize (lang, source, verbose) ;
		
		//preprocessor here (one pass preprocessor)
		lang.tokens = preproc->preprocess (filename, lang.tokens) ; //segfault here

		// text tokens are needed for next step
		std::string txtok = utils::transformtosource (lang.tokens) ; 
		
		// tokenize in abstract grammar tokens
		lexer::Lexer gram (operations::GenericGrammarTokenizer::genericgrammarprodrules) ;
		
		gram = operations::GenericGrammarTokenizer::tokenize (gram, txtok, verbose) ;

		// make production rules
		Grammar grammar = Grammar () ;

		grammar.makegrammar (
			gram.tokens,
			lang.tokens
		) ;

		out_grammar.merge (grammar) ;
	}
	out_grammar = normoperators::get2nf(out_grammar) ;
	if (verbose) {
		cout << out_grammar ;
	}
	return out_grammar ;
}

} // parsers namespace 

} // parselib namespace


