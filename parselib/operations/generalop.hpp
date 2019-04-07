#pragma once

#include <parselib/parsers/grammarparser.hpp>
#include <parselib/operations/naiveparsers.hpp>

namespace parselib {
	
namespace grammaroperators {

/*! \brief eliminates duplicate rules in a grammar
 * \param grammar : grammar in
 * \returns grammar : grammar out
 */
myparsers::Grammar eliminatedoubles (myparsers::Grammar grammar) ;

/*!
 * \brief check if rule already exists
 * \param uniquerules : (Rules) list of unique rules
 * \param rule : rule to check
 * \returns true or false (bool)
 */
bool checkunique (operations::SequentialParser::Rules uniquerules, operations::SequentialParser::Rule rule) ;

/*!
 * \brief check is rule a & b are the same
 * \param a
 * \param b : rules to compare
 * \returns bool
 */
bool samerule (operations::SequentialParser::Rule rulea, operations::SequentialParser::Rule ruleb) ;

}


}


