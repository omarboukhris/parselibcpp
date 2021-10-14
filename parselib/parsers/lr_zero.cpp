
#include <string>

#include "lr_zero.hpp"

namespace parselib {

namespace parsers {

LR_zero::LR_zero (Grammar grammar) {
	production_rules = grammar.production_rules ;
	unitrelation = grammar.unitrelation ;
	build_table();
}

void LR_zero::build_table(){

	if (production_rules.size() < 1) {
		return ;
	}

	Rule axiom_rule = production_rules["AXIOM"].at(0);
	Item axiom (axiom_rule);
	Closure i0 = make_closure(axiom);

	std::cout << "-----------------" << std::endl;

	m_graph.push_back(i0);

	Closure clos = i0;

	bool keep_going = true;

	while(keep_going) {

		keep_going = false;

		for (Item current_item : clos) {

			while(not current_item.done()) {

				std::cout << "-----------------" << current_item.getPosition() << ":" << current_item.getRule().size() << std::endl;
				// add transition here
				Closure newest_clos = make_closure(current_item);

				if (newest_clos.empty()) {
					break ;
				}

				auto it = std::find(m_graph.begin(), m_graph.end(), newest_clos);
				if (it == m_graph.end()) {
					std::cout << "nexist" << std::endl ;
					// add the new item to closures
					newest_clos.add_transition(clos.label());
					m_graph.push_back(newest_clos);
					keep_going = true;

					// add newest_clos to processing queue

				} else {
					std::cout << "exis" << std::endl ;
					it->add_transition(clos.label());
					// make transition from clos to existing node (*it)
					// <clos.label, current_item.read()>
				}
			}
		}

		// clos = ? current one should be done
	}
}

Closure LR_zero::make_closure(Item &current_item){

	Closure output;

	// this is where the magic happens
	Token token = current_item.readNext();
	if (not token.key().size()) {
		return output;
	}

	output.add_item(current_item);

	std::cout << token.key() << token.value() << "$$" << current_item.done() << std::endl ;

	if (not current_item.done()) {

		// second is term/non-term
		std::string rulename = token.value();
		std::cout << "rulename :" << rulename << std::endl ;

		// queue & processed list to avoid endless looping
		std::vector<std::string> q;
		std::vector<std::string> processed;

		if (token.type() != "TERMINAL") {
			// process it only if non terminal
			q.push_back(rulename);
		}

		while (q.size()) {

			// get token name to process
			rulename = q.back();
			std::cout << "q:" << q.size() << ":" << rulename << ":" << std::endl ;
			q.pop_back();

			// if rule unprocessed
			auto itRule = std::find(processed.begin(), processed.end(), rulename) ;
			if (itRule == processed.end()) {

				processed.push_back(rulename);

				std::cout << rulename << ":" << production_rules[rulename].size() << std::endl ;

				// put rule tokens in queue for processing
				for (Rule rule: production_rules[rulename]) {

					Item new_item (rule);
					output.add_item(new_item);

					for (Token tok: rule) {
						std::cout << "\t" << tok.value() << std::endl ;
						auto it = std::find(processed.begin(), processed.end(), tok.value());
						if (it == processed.end() and tok.type() != "TERMINAL" and tok.type() != "EMPTY")
						{
							std::cout << tok.value() << ":" << tok.key() << std::endl ;
							q.push_back(tok.value());
						}
					}
				}
			}

		}

	}

	return output;
}

}

}
