
#include "generalop.hpp"

#include "normop.hpp"


namespace parselib::normoperators {


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
		StrVect rulebyname = StrVect() ;
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
std::set<std::string> getnullables (const Grammar& grammar) {
	ProductionRules production_rules = grammar.production_rules ;

    // construct occurences map for back propagation of nullables
    auto occurs = std::map<std::string, std::vector<StrVect>>();
    for (const auto& item : production_rules) {
        std::string key = item.first;
        Rules rules = item.second;

        for (const auto& rule: rules) {
            if (rule.size() == 1) {
                if (occurs.find(rule[0].cvalue()) == occurs.end())
                    occurs[rule[0].cvalue()] = std::vector<StrVect>();
                occurs[rule[0].cvalue()].push_back({key});
            }
            else if (rule.size() == 2) {
                if (occurs.find(rule[0].cvalue()) == occurs.end())
                    occurs[rule[0].cvalue()] = std::vector<StrVect>();
                if (occurs.find(rule[1].cvalue()) == occurs.end())
                    occurs[rule[1].cvalue()] = std::vector<StrVect>();
                occurs[rule[0].cvalue()].push_back(StrVect({key, rule[1].cvalue()}));
                occurs[rule[1].cvalue()].push_back(StrVect({key, rule[0].cvalue()}));
            }
        }

    }

    // prepare todo_ queue
    auto nullables = std::set<std::string>() ;
    auto todo = std::vector<std::string>();
	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		for (Rule rule : rules) {

			bool isruleempty = (rule.size() == 1 && rule[0].type() == Token::Empty) ;
			if (isruleempty) {
				nullables.emplace(key) ;
                todo.push_back(key);
			}
		}
	}

    // construct and propagate nullables
    while(not todo.empty()) {
        const auto& occ = occurs[todo.back()];
        todo.pop_back();

        for (const auto& rule : occ) {
            if (nullables.find(rule.back()) != nullables.end() and nullables.find(rule.front()) == nullables.end()) {
                nullables.emplace(rule.front());
                todo.push_back(rule.front());
            }
        }
	}
	return nullables ;
}

/*!
 * \brief calculates unit relations (used in cyk2nf) in a grammar
 * may cause bugs? to check
 * Re-read the paper one of these days
 * \param grammar : grammar
 * \return grammar 
 */
Grammar getunitrelation (Grammar grammar) {
    /* this introduces major perf issues -- probably too much units ?? should be fixed
    // current fix: reverting to previous unit computation
	std::set<std::string> nullables = getnullables (grammar) ;

	ProductionRules production_rules = grammar.production_rules ;

	Grammar::UnitRelation unitrelation = Grammar::UnitRelation() ;

	for (const auto& item : production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;

		for (const Rule& rule : rules) {

            if (rule.size() == 1) {
				unitrelation[rule[0].cvalue()].push_back(key) ;
            }

            else if (rule.size() == 2) {
                if (nullables.find(rule[0].ctype()) == nullables.end()) {
					unitrelation[rule[0].cvalue()].push_back (key) ;
                }
                if (nullables.find(rule[1].ctype()) == nullables.end()) {
					unitrelation[rule[1].cvalue()].push_back (key) ;
                }
            }
		}
	}
    //*/

    auto nullables = getnullables (grammar) ;

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
            StrList epsOrTerminal = StrList({"EMPTY", "TERMINAL"}) ;
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
//            bool isruleunit = (std::find(nullables.begin(), nullables.end(), rule[0].value()) != nullables.end()) ;
            bool isruleunit = nullables.find(rule[0].value()) != nullables.end() ;
            if (isruleunit) {
                if (std::find(unitkeylist.begin(), unitkeylist.end(), key) != unitkeylist.end()) {
                    unitrelation[key].push_back (rule[1].value()) ;
                } else {
                    unitrelation[key] = StrList({rule[1].value()}) ;
                    unitkeylist.push_back(key);
                }
            }
            isruleunit = nullables.find(rule[1].value()) != nullables.end() ;
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

 	grammar.unitrelation = unitrelation;
	return grammar ;
}

} // namespace parselib::normoperators



	
