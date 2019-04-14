#include <parselib/operations/normop.hpp>

#include <parselib/operations/generalop.hpp>

using namespace std ;

namespace parselib {

using namespace operations ;

namespace normoperators {

myparsers::Grammar get2nf(myparsers::Grammar grammar) {
	SequentialParser::ProductionRules production_rules = grammar.production_rules ;
	TERM term (production_rules) ;
	term.apply() ;
	BIN bin (term.production_rules) ;
	bin.apply() ;
	grammar.production_rules = bin.production_rules ;
	grammar = grammaroperators::eliminatedoubles(grammar) ;
	grammar = getunitrelation (grammar) ;
	grammar = removenullables (grammar) ;
	return grammar ;
}

TERM::TERM (SequentialParser::ProductionRules production_rules) {
	this-> production_rules = production_rules ;
	this->normalForm = SequentialParser::ProductionRules () ;
}

void TERM::apply() {
	term () ;
}

void TERM::term() {
	
	for (auto item : production_rules) {
		string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		if (production_rules.find(key) == production_rules.end()) {
			normalForm[key] = SequentialParser::Rules() ;
		}
		for (SequentialParser::Rule rule : rules) {
			checkruleforterminals(key, rule);
		}
	}
	production_rules = normalForm ;
}

void TERM::checkruleforterminals(string key, SequentialParser::Rule rule) {
	SequentialParser::Rule newRule = SequentialParser::Rule() ;
	for (lexer::Lexer::Token operand : rule) {
		if (operand.second == "TERMINAL") {
			string newKey = operand.first + "." ;
			if (normalForm.find(newKey) == normalForm.end()) {
				normalForm[newKey] = SequentialParser::Rules() ;
			}
			newRule.push_back(lexer::Lexer::Token(newKey, "NONTERMINAL"));
			normalForm[newKey].push_back({operand});
		} else {
			newRule.push_back(operand);
		}
	}
	normalForm[key].push_back(newRule);
}

BIN::BIN(SequentialParser::ProductionRules production_rules) {
	this-> production_rules = production_rules ;
	this->normalForm = SequentialParser::ProductionRules () ;
}

void BIN::apply() {
	binarize () ;
}

void BIN::binarize() {
	while (binonce()) ;
}

bool BIN::binonce() {
	normalForm = SequentialParser::ProductionRules() ;
	bool changed = false ;
	for (auto item : production_rules) {
		string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		
		if (normalForm.find(key) == normalForm.end()) {
			normalForm[key] = SequentialParser::Rules() ;
		}
		for (SequentialParser::Rule rule : rules) {
			BIN::binarizerule(key, rule) ;
			if (rule.size() > 2) {
				changed = true ;
			}
		}
	}
	production_rules = normalForm ;
	return changed ;
}

void BIN::binarizerule(string key, SequentialParser::Rule rule) {
	if (rule.size() <= 2) {
		normalForm[key].push_back(rule);
	} else {
		SequentialParser::Rule newRule = SequentialParser::Rule () ;
		SequentialParser::StrList rulebyname = SequentialParser::StrList() ;
		bool dirty = true ;
		for (lexer::Lexer::Token token : rule) {
			if (dirty) {
				dirty = false ;
				continue ;
			}
			rulebyname.push_back(token.first) ;
			newRule.push_back(token) ;
		}
		string newKey = utils::join(rulebyname, "/") ;
		if (normalForm.find(newKey) == normalForm.end()) {
			normalForm[newKey] = SequentialParser::Rules () ;
		}
		normalForm[key].push_back({
			rule[0], lexer::Lexer::Token(newKey, "NONTERMINAL")
		});
		normalForm[newKey].push_back(newRule);
	}
}

myparsers::Grammar removenullables (myparsers::Grammar grammar) {
	SequentialParser::ProductionRules production_rules = SequentialParser::ProductionRules() ;
	for (auto item : grammar.production_rules) {
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		production_rules[key] = SequentialParser::Rules() ;
		for (SequentialParser::Rule rule : rules) {
			if (rule.size() == 1 && rule[0].second == "EMPTY") {
				continue ;
			}
			production_rules[key].push_back(rule) ;
		}
	}
	grammar.production_rules = production_rules ;
	return grammar ;
}

/*!
 * \brief only if binned (less of a headache to implement)
 * returns a list of all nullable rules in a grammar
 * \param grammar : grammar input
 * \return list of unique nullables
 */
SequentialParser::StrList getnullables (myparsers::Grammar grammar) {
	SequentialParser::ProductionRules production_rules = grammar.production_rules ;
	
	SequentialParser::StrList nullables = SequentialParser::StrList() ;
	size_t lenG = 0 ;
	for (auto item : production_rules) {
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;
		for (SequentialParser::Rule rule : rules) {
			lenG += 1 ;
			
			bool isruleempty = (rule.size() == 1 && rule[0].second == "EMPTY") ;
			if (isruleempty) {
				nullables.push_back (key) ;
			}
		}
	}

	for (size_t i = 0 ; i < lenG ; i++) {
		for (auto item : production_rules) {
			std::string key = item.first ;
			SequentialParser::Rules rules = item.second ;
			
			for (SequentialParser::Rule rule : rules) {
				bool isruleempty = (
					std::find(nullables.begin(), nullables.end(), rule[0].first) != nullables.end() &&
					std::find(nullables.begin(), nullables.end(), rule[1].first) != nullables.end()) ;
				if (isruleempty) {
					nullables.push_back (key) ;
				}
			}
		}
	}
	
	//remove duplicates
	SequentialParser::StrList nulls = SequentialParser::StrList() ;
	for (std::string str : nullables) {
		if (std::find (nulls.begin(), nulls.end(), str) == nulls.end()) {
			nulls.push_back(str);
		}
	}
	return nulls ;
}

/*!
 * \brief calculates unit relations (used in cyk2nf) in a grammar
 * may cause bugs? to check
 * Re-read the paper one of these days
 * \param grammar : grammar
 * \return grammar 
 */
myparsers::Grammar getunitrelation (myparsers::Grammar grammar) {
	SequentialParser::StrList nullables = getnullables (grammar) ;

	SequentialParser::ProductionRules production_rules = grammar.production_rules ;

	myparsers::Grammar::UnitRelation unitrelation = myparsers::Grammar::UnitRelation() ;

	SequentialParser::StrList unitkeylist = SequentialParser::StrList() ;
	//first pass
	for (auto item : production_rules) {
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;

		for (SequentialParser::Rule rule : rules) {
			SequentialParser::StrList epsOrTerminal = SequentialParser::StrList({"EMPTY", "TERMINAL"}) ;
			bool isruleunit = (
				rule.size() == 1 && 
				(std::find(epsOrTerminal.begin(), epsOrTerminal.end(), rule[0].second) == epsOrTerminal.end())
			) ;
			if (isruleunit) {
				if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
					unitrelation[key].push_back (rule[0].first) ;
				} else {
					unitrelation[key] = SequentialParser::StrList({rule[0].first}) ;
					unitkeylist.push_back(key);
				}
			}
		}
	}

	//second pass
	for (auto item : production_rules) {
		std::string key = item.first ;
		SequentialParser::Rules rules = item.second ;

		for (SequentialParser::Rule rule : rules) {
			if (rule.size() != 2) {
				continue ;
			}
			bool isruleunit = (std::find(nullables.begin(), nullables.end(), rule[0].first) == nullables.end()) ;
			if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
				unitrelation[key].push_back (rule[1].first) ;
			} else {
				unitrelation[key] = SequentialParser::StrList({rule[1].first}) ;
				unitkeylist.push_back(key);
			}
  			isruleunit = (std::find(nullables.begin(), nullables.end(), rule[1].first) != nullables.end()) ;
			if (isruleunit) {
				if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
					unitrelation[key].push_back (rule[0].first) ;
				} else {
					unitrelation[key] = SequentialParser::StrList({rule[0].first}) ;
					unitkeylist.push_back(key);
				}
			}
		}
	}

	myparsers::Grammar::UnitRelation outunit = myparsers::Grammar::UnitRelation () ;
	for (auto item : unitrelation) {
		std::string key = item.first ;
		SequentialParser::StrList unitrel = item.second ;
		outunit[key] = SequentialParser::StrList () ;
		for (std::string unit : unitrel) {
			if (std::find(outunit[key].begin(), outunit[key].end(), unit) == outunit[key].end()) {
				//key doesn't exist
				outunit[key].push_back(unit);
			}
		}
	}
	
 	grammar.unitrelation = outunit ;
	return grammar ;
}

} //namespace normoperators

} //namespace parselib

	
