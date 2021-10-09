#pragma once

#include <parselib/datastructure/common_types.h>
#include <parselib/parsers/grammarparser.hpp>

namespace parselib {

namespace normoperators {

/*!
 * \brief transform grammar to 2NF form
 * \param[in] grammar  Input grammar
 * \return output grammar
 */
Grammar get2nf (Grammar grammar) ;

/*!
 * \brief removenullables removes nullables from grammar
 * \param[in] grammar  Input grammar
 * \return output grammar
 */
Grammar removenullables (Grammar grammar) ;

/*!
 * \brief getnullables from a grammar
 * \param[in] grammar  Input grammar
 * \return nullables rule labels in a vector
 */
StrList getnullables (Grammar grammar) ;

/*!
 * \brief getunitrelation from grammar
 * \param grammar  Input grammar
 * \return unit relations in a grammar
 */
Grammar getunitrelation (Grammar grammar) ;

/*!
 * \brief The TERM class applies term operator
 * on a grammar's production rules
 */
class TERM {
public :

	/// \brief internal production rules
	ProductionRules production_rules ;

	/*!
	 * \brief TERM class constructor
	 * \param production_rules input grammar's production rules
	 */
	TERM (ProductionRules production_rules) ;

	/*!
	 * \brief apply operator
	 */
	void apply () ;

private :
	ProductionRules normalForm ;
	
	void term () ;

	/*!
	 * \brief checkruleforterminals
	 * \param key   rule label
	 * \param rule  rule as a list of tokens
	 */
	void checkruleforterminals (std::string key, Rule rule) ;
} ;

/*!
 * \brief The BIN class serves as an operator
 * to make binary grammar
 */
class BIN {
public :

	/// \brief internal production rules
	ProductionRules production_rules ;

	/*!
	 * \brief BIN class construction
	 * \param production_rules  input grammar's production rule
	 */
	BIN (ProductionRules production_rules) ;

	/*!
	 * \brief apply operator
	 */
	void apply () ;

private :

	ProductionRules normalForm ;

	void binarize () ;
	bool binonce () ;

	void binarizerule (std::string key, Rule rule) ;
} ;


} //namespace normoperators

} //namespace parselib
