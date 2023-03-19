
#include <string>

#include "lr_zero.hpp"

namespace parselib::parsers {

LR_zero::LR_zero (const Grammar& grammar) {
	production_rules = grammar.production_rules ;
	unitrelation = grammar.unitrelation ;
	build_table();
}

void LR_zero::build_table(){

	if (production_rules.empty()) {
		return ;
	}
    int j = 0;
	Rule axiom_rule = production_rules["AXIOM"].at(0);
	Item axiom (axiom_rule);
	Closure i0 = make_closure(j++, axiom);

	std::vector<Closure> stack;

	m_graph.push_back(i0);
	stack.push_back(i0);

	while(not stack.empty()) {
		Closure clos(stack.back());

		for (Item current_item : clos) {

			while(not current_item.done()) {
                // add transition here
                current_item.next();
				Closure newest_clos = make_closure(j++, current_item);

				if (newest_clos.empty()) {
					break ;
				}

				auto it = std::find(m_graph.begin(), m_graph.end(), newest_clos);
				if (it == m_graph.end()) {
					// add the new item to closures
					newest_clos.add_transition(clos.label());
					m_graph.push_back(newest_clos);

					// add newest_clos to processing stack
					stack.push_back(newest_clos);

				} else {
					it->add_transition(clos.label());
					// make transition from clos to existing node (*it)
					// <clos.label, current_item.read()>
				}
			}

            // once rule is fully read, make final state, add transitions
            Closure final_state = Closure::last_state(j++, current_item);
            auto it = std::find(m_graph.begin(), m_graph.end(), final_state);
            if (it == m_graph.end()) {
                // add the new item to closures
                final_state.add_transition(clos.label());
                m_graph.push_back(final_state);
            } else {
                it->add_transition(clos.label());
            }
		}
		stack.pop_back();
	}
}

Closure LR_zero::make_closure(int id, Item &current_item) {

	Closure output(id);

	// this is where the magic happens
	Token token = current_item.read();
	if (token.key().empty()) {
		return output;
	}

	output.add_item(current_item);

    // second is term/non-term
    std::string rulename = token.value();

    // queue & processed list to avoid endless looping
    std::vector<std::string> q, processed;

    if (token.type() != "TERMINAL") {
        // process it only if non terminal
        q.push_back(rulename);
    }

    while (not q.empty()) {
        // get token name to process
        rulename = q.back();
        q.pop_back();

        // if rule unprocessed
        auto itRule = std::find(processed.begin(), processed.end(), rulename) ;
        if (itRule == processed.end()) {
            // put rule tokens in queue for processing
            for (const Rule& rule: production_rules[rulename]) {
                Item new_item (rule);
                output.add_item(new_item);

                for (Token tok: rule) {
                    auto it = std::find(processed.begin(), processed.end(), tok.value());
                    if (it == processed.end() and tok.type() != "TERMINAL" and tok.type() != "EMPTY")
                    {
                        q.push_back(tok.value());
                    }
                }
            }
            processed.push_back(rulename);
        }
    }

	return output;
}

void LR_zero::shift_reduce() {

}

}
