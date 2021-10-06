#include <parselib/operations/generalop.hpp>

namespace parselib {
	
using namespace operations ;

namespace grammaroperators {

/*! 
 * \brief eliminates duplicate rules in a grammar
 * \param grammar : grammar in
 * \returns grammar : grammar out
 */
myparsers::Grammar eliminatedoubles (myparsers::Grammar grammar) {

	ProductionRules production_rules = ProductionRules () ;
	for (auto item : grammar.production_rules) {
		std::string key = item.first ;
		Rules rules = item.second ;
		
		Rules uniquerules = Rules() ;
		for (Rule rule : rules) {
			bool exist = checkunique (uniquerules, rule) ;
			if (!exist) {
				uniquerules.push_back(rule);
			}
		}
		production_rules[key] = uniquerules ;
	}
	grammar.production_rules = production_rules ;
	return grammar ;

}

/*!
 * \brief check if rule already exists
 * \param uniquerules : (Rules) list of unique rules
 * \param rule : rule to check
 * \returns true or false (bool)
 */
bool checkunique (Rules uniquerules, Rule rule) {
	for (Rule r : uniquerules) {
		if (samerule (r, rule)) {
			return true ;
		}
	}
	return false ;
}
	
/*!
 * \brief check is rule a & b are the same
 * \param a
 * \param b : rules to compare
 * \returns bool
 */
bool samerule (Rule rulea, Rule ruleb) {
	if (rulea.size() == ruleb.size()) {
		Rule::iterator opa = rulea.begin() ;
		Rule::iterator opb = ruleb.begin() ; 
		
		while (opa != rulea.end() && opb != ruleb.end()) {
			if (!(opa->second == opb->second && 
				opa->first == opb->first)) { 
				return false ;
			}
			opa++ ; opb++ ;
		}
		return true ;
	} else {
		return false ;
	}
}

}


}



