#pragma once

#include <parselib/datastructure/common_types.h>
#include <parselib/datastructure/grammar.h>

#include <parselib/parsers/lexer.hpp>

namespace parselib {
	
namespace grammaroperators {

class GenericGrammarTokenizer {

public :

	/// \brief regex map used to tokenize text grammar
	static PatternsMap grammartokens ;

	/// \brief meta grammar production rules
	static PatternsMap genericgrammarprodrules ;

} ;

/*!
 * \brief eliminates duplicate rules in a grammar
 * \param grammar : grammar in
 * \returns grammar : grammar out
 */
Grammar eliminatedoubles (Grammar grammar) ;

}

/*!
 * \brief check if rule already exists
 * \param uniquerules : (Rules) list of unique rules
 * \param rule : rule to check
 * \returns true or false (bool)
 */
bool checkunique (const Rules& uniquerules, const Rule& rule) ;

/*!
 * \brief check is rule a & b are the same
 * \param a
 * \param b : rules to compare
 * \returns bool
 */
bool samerule (Rule rulea, Rule ruleb) ;

}

