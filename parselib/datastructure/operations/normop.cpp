
#include <parselib/datastructure/operations/generalop.hpp>

#include "normop.hpp"


namespace parselib {

namespace normoperators {


Grammar get2nf(Grammar grammar) {
	ProductionRules production_rules = grammar.production_rules ;
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

TERM::TERM (const ProductionRules &production_rules) {
	this-> production_rules = production_rules ;
	this->normalForm = ProductionRules () ;
}

void TERM::apply() {
	term () ;
}

void TERM::term() {
	
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		if (production_rules.find(key) == production_rules.end()) {
			normalForm[key] = Rules() ;
		}
		for (const Rule& rule : rules) {
			checkruleforterminals(key, rule);
		}
	}
	production_rules = normalForm ;
}

void TERM::checkruleforterminals(const std::string& key, const Rule& rule) {
	Rule newRule = Rule() ;
	for (Token operand : rule) {
		if (operand.type() == Token::Terminal) {
			std::string newKey = operand.value() + "." ;
			if (normalForm.find(newKey) == normalForm.end()) {
				normalForm[newKey] = Rules() ;
			}
			newRule.emplace_back(newKey, Token::NonTerminal);
			normalForm[newKey].push_back({operand});
		} else {
			newRule.push_back(operand);
		}
	}
	normalForm[key].push_back(newRule);
}

BIN::BIN(const ProductionRules &production_rules) {
	this-> production_rules = production_rules ;
	this->normalForm = ProductionRules () ;
}

void BIN::apply() {
	binarize () ;
}

void BIN::binarize() {
	while (binonce()) ;
}

bool BIN::binonce() {
	normalForm = ProductionRules() ;
	bool changed = false ;
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		
		if (normalForm.find(key) == normalForm.end()) {
			normalForm[key] = Rules() ;
		}
		for (const Rule& rule : rules) {
			BIN::binarizerule(key, rule) ;
			if (rule.size() > 2) {
				changed = true ;
			}
		}
	}
	production_rules = normalForm ;
	return changed ;
}

void BIN::binarizerule(const std::string& key, Rule rule) {
	if (rule.size() <= 2) {
		normalForm[key].push_back(rule);
	} else {
		Rule newRule = Rule () ;
		StrList rulebyname = StrList() ;
		bool dirty = true ;
		for (Token token : rule) {
			if (dirty) {
				dirty = false ;
				continue ;
			}
			rulebyname.push_back(token.value()) ;
			newRule.push_back(token) ;
		}
		std::string newKey = utils::join(rulebyname, "/") ;
		if (normalForm.find(newKey) == normalForm.end()) {
			normalForm[newKey] = Rules () ;
		}
		normalForm[key].push_back({
			rule[0], Token(newKey, Token::NonTerminal)
		});
		normalForm[newKey].push_back(newRule);
	}
}

Grammar  removenullables (Grammar grammar) {
	ProductionRules production_rules = ProductionRules() ;
	for (const auto& item : grammar.production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		production_rules[key] = Rules() ;
		for (Rule rule : rules) {
			if (rule.size() == 1 && rule[0].type() == Token::Empty) {
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
StrList getnullables (const Grammar& grammar) {
	ProductionRules production_rules = grammar.production_rules ;
	
	StrList nullables = StrList() ;
	size_t lenG = 0 ;
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		for (Rule rule : rules) {
			lenG += 1 ;
			
			bool isruleempty = (rule.size() == 1 && rule[0].type() == Token::Empty) ;
			if (isruleempty) {
				nullables.push_back (key) ;
			}
		}
	}

	for (size_t i = 0 ; i < lenG ; i++) {
		for (const auto& item : production_rules) {
			std::string key = item.first ;
			Rules rules = item.second ;
			
			for (Rule rule : rules) {
				if (rule.size() != 2) {
					continue ;
				}
				bool isruleempty = (
					std::find(nullables.begin(), nullables.end(), rule[0].value()) != nullables.end() &&
					std::find(nullables.begin(), nullables.end(), rule[1].value()) != nullables.end()) ;
				if (isruleempty) {
					nullables.push_back (key) ;
				}
			}
		}
	}
	
	//remove duplicates
	StrList nulls = StrList() ;
	for (const std::string& str : nullables) {
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
Grammar getunitrelation (Grammar grammar) {
	StrList nullables = getnullables (grammar) ;

	ProductionRules production_rules = grammar.production_rules ;

	Grammar::UnitRelation unitrelation = Grammar::UnitRelation() ;

	StrList unitkeylist = StrList() ;
	//first pass
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;

		for (Rule rule : rules) {
			if (rule.size() != 1) {
				continue ;
			}
			StrList epsOrTerminal = StrList({Token::Empty, Token::Terminal}) ;
			bool isruleunit = (
				(std::find(epsOrTerminal.begin(), epsOrTerminal.end(), rule[0].type()) == epsOrTerminal.end())
			) ;
			if (isruleunit) {
				if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
					unitrelation[key].push_back (rule[0].value()) ;
				} else {
					unitrelation[key] = StrList({rule[0].value()}) ;
					unitkeylist.push_back(key);
				}
			}
		}
	}

	//second pass
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;

		for (Rule rule : rules) {
			if (rule.size() != 2) {
				continue ;
			}
			bool isruleunit = (std::find(nullables.begin(), nullables.end(), rule[0].value()) != nullables.end()) ;
			if (isruleunit) {
				if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
					unitrelation[key].push_back (rule[1].value()) ;
				} else {
					unitrelation[key] = StrList({rule[1].value()}) ;
					unitkeylist.push_back(key);
				}
			}
			isruleunit = (std::find(nullables.begin(), nullables.end(), rule[1].value()) != nullables.end()) ;
			if (isruleunit) {
				if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
					unitrelation[key].push_back (rule[0].value()) ;
				} else {
					unitrelation[key] = StrList({rule[0].value()}) ;
					unitkeylist.push_back(key);
				}
			}
		}
	}

	Grammar::UnitRelation outunit = Grammar::UnitRelation () ;
	for (const auto& item : unitrelation) {
		std::string key = item.first ;
		StrList unitrel = item.second ;
		outunit[key] = StrList () ;
		for (const std::string& unit : unitrel) {
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

	
