
#include <parselib/utils/io.hpp>

#include "seqparsers.hpp"

using namespace std ;

namespace parselib::parsers {

SequentialParser::SequentialParser (
    const TokenList &grammar,
    const TokenList &parsedtokens)
: op_count(0), tok_count(0) {

	axiomflag = true ;

	production_rules = ProductionRules() ;

	strnodes = StrMap () ;

	keeper = KeepingList() ;
	keeper["all"] = StrVect() ;

	labels = LabelReplacementMap() ;

	tokens = TokenList () ;

	this->grammar = grammar ;
	this->parsedtokens = parsedtokens ;
}

void SequentialParser::parse () {
	while (stillparsing()) {
		check_axiom () ;
		check_left_side() ;
		check_right_side() ;
		check_operators () ;
		check_for_token() ;
	}
}

void SequentialParser::check_axiom () {
	size_t
		i = this->op_count,
		j = this->tok_count ;
	
	if (i >= grammar.size()) {
		return ;
	}
	
	// grammar(i).first = value,
	//			 .second = type
	if (grammar[i].type() == Token::Axiom && axiomflag) {
		Token axiom = parsedtokens[j+2] ;
		production_rules[Token::Axiom] = {{axiom}} ;
		axiomflag = false ;
		i += 1 ;
		j += 3 ;
	}
	this->op_count = i ;
	this->tok_count = j ;
}

void SequentialParser::check_for_token () {
	size_t
		i = this->op_count,
		j = this->tok_count ;
	
	if (i >= grammar.size()) {
		return ;
	}
	
	if (grammar[i].type() == "TOKEN") {
		
		//eliminate the dot @token value
		std::string label = utils::clean_if_terminal(parsedtokens[j].value()) ;
		//eliminate the parentheses+quotes
		std::string regex = utils::clean_regex(parsedtokens[j+1].value()) ;

		tokens.emplace_back(regex, label) ;
		i += 1 ;
		j += 2 ;
	}
	this->op_count = i ;
	this->tok_count = j ;
}

void SequentialParser::check_left_side () {
	size_t
		i = this->op_count,
		j = this->tok_count ;
	
	if (i >= grammar.size()) {
		return ;
	}
	if (grammar[i].type() == "LSIDE") {
		current_rule = parsedtokens[j].value() ;
		
		// if current_rule in production_rules.keys()
		if (production_rules.find (current_rule) == production_rules.end()) {
			production_rules[current_rule] = Rules() ;
		}
		
		i += 1 ;
		j += 2 ;
	}
	this->op_count = i ;
	this->tok_count = j ;
}

void SequentialParser::check_operators () {
	size_t
		i = this->op_count,
		j = this->tok_count ;
	
	if (i >= grammar.size()) {
		return ;
	}

	if (grammar[i].type() == "OR" &&
		parsedtokens[j].type() == "OR") {

		//add a new rule (list of tokens)
		production_rules[current_rule].emplace_back() ;
		j += 1 ;
		i += 1 ;
	}
	if (grammar[i].type()  == "LINECOMMENT" &&
		parsedtokens[j].type() == "LINECOMMENT") {
		j += 1 ;
		i += 1 ;
	}
	this->op_count = i ;
	this->tok_count = j ;
}

void SequentialParser::check_right_side() {
	size_t
		i = this->op_count,
		j = this->tok_count ;
	
	if (i >= grammar.size()) {
		return ;
	}

	while (i < grammar.size() && grammar[i].type() == "RSIDE") {

		if (parsedtokens[j].type() == Token::Terminal) {
			parsedtokens[j].value() = utils::clean_if_terminal(parsedtokens[j].value()) ;
		}

        if (parsedtokens[j].type() == std::string("STR")) {
			add_to_str_rules(j) ;
			add_to_keeper(j) ;
		}
        else if (parsedtokens[j].type() == "LIST") {
            make_list() ;
        }
        else if (parsedtokens[j].type() == "LISTOP") {
            make_list_op() ;
        }
        else if (parsedtokens[j].type() == "EXCL") {
			
			// naming process
			std::string label = parsedtokens[j+1].value() ;
			label = utils::clean_if_terminal(label) ;
			parsedtokens[j].value() = label ;

			process_label (label, label) ;
			add_to_keeper(j) ;
		}
        else {
			if (parsedtokens[j].value().find('=') != std::string::npos) {
				//naming process
				StrVect out = utils::split(parsedtokens[j].value(), "=") ;
				std::string label, operand ;
				
				if (out.size() == 2) {
					label = out[0] ;
					operand = out[1] ;
				} else {
					//error happened
					return ;
				}

				parsedtokens[j].value() = operand ;

				process_label(label, operand) ;
			}
			
			production_rules = add_operand_to_current_rule(parsedtokens[j]) ;
		}
		i += 1 ;
		j += 1 ;
	}

	this->op_count = i ;
	this->tok_count = j ;
}

void SequentialParser::process_label(const std::string& label, const std::string& operand){
	//currentrule in labels.keys()
	if (labels.find (current_rule) != labels.end()) {
		labels[current_rule][operand] = label ;
	} else {
		labels[current_rule] = LabelReplacement() ;
		labels[current_rule][operand] = label ;
	}

	if (keeper.find(current_rule) != keeper.end()) {
		keeper[current_rule].push_back(label) ;
	} else {
		keeper[current_rule] = StrVect() ;
		keeper[current_rule].push_back(label) ;
	}
}

void SequentialParser::make_list_op() {
    // parse parent node, element node and separator out of regex
    // add rule parent node = element | element + parent node
    // add element node = element | element sep element node
}

void SequentialParser::make_list(){
    /// note : try not to use epsilone rules when they can be avoided
    /// they needlessly inflate the grammar and thus processing time
	Token thisnode (current_rule, std::string(Token::NonTerminal)) ;
//	Token eps (std::string("''"), std::string(Token::Empty)) ;
	production_rules[current_rule].back() = {thisnode, thisnode} ;
//	production_rules[current_rule].push_back({eps}) ;
}

ProductionRules SequentialParser::add_operand_to_current_rule(const Token& tok) {
	// adding an operand to the current running rule
	if (!production_rules[current_rule].empty()) {
		production_rules[current_rule].back().push_back(tok) ;
	} else {
		production_rules[current_rule].push_back({tok}) ;
	}
	return production_rules ;
}

void SequentialParser::add_to_keeper(size_t j) {
	string entry = utils::clean_if_terminal(parsedtokens[j + 1].value()) ;
	if (keeper.find(current_rule) != keeper.end()) {
		if (std::find(keeper[current_rule].begin(), keeper[current_rule].end(), entry) == keeper[current_rule].end()) {
			keeper[current_rule].push_back(entry);
		}
	} else {
		keeper[current_rule] = StrVect() ;
		keeper[current_rule].push_back(entry) ;
	}
}

void SequentialParser::add_to_str_rules(size_t j) {
	std::string nodename = utils::clean_if_terminal(parsedtokens[j + 1].value()) ;
    StrVect &strnode = strnodes[current_rule];
	// add to strnodes
	if (std::find(strnode.begin(), strnode.end(), nodename) == strnode.end()) {
		strnode.push_back(nodename) ;
	}
}

} //namespace parselib



