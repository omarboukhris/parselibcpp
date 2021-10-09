
#include "lr_zero.hpp"


namespace parselib {

namespace myparsers {

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

	std::vector<Closure> graph;
	graph.push_back(i0);

	Closure clos = i0;

	bool keep_going = true;

	while(keep_going) {

		keep_going = false;

		for (Item current_item : clos) {

			while(not current_item.done()) {

				// add transition here
				Closure newest_clos = make_closure(current_item);

				auto it = std::find(graph.begin(), graph.end(), newest_clos);
				if (it == graph.end()) {
					// add the new item to closures
					graph.push_back(newest_clos);
					keep_going = true;

				} else {

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
	output.push_back(current_item);

	if (not current_item.done()) {

		// second is term/non-term
		std::string rulename = token.first;

		// queue & processed list to avoid endless looping
		std::queue<std::string> q;
		std::vector<std::string> processed;

		if (token.second != "TERMINAL") {
			// process it only if non terminal
			q.push(rulename);
		}

		while (not q.empty()) {

			// get token name to process
			rulename = q.front();
			q.pop();

			// if rule unprocessed
			if (std::find(processed.begin(), processed.end(), rulename) == processed.end()) {

				processed.push_back(rulename);

				// put rule tokens in queue for processing
				for (Rule rule: production_rules[rulename]) {

					Item new_item (rule);
					output.push_back(new_item);

					for (Token tok: rule) {

						if (std::find(processed.begin(), processed.end(), tok.first) == processed.end()
							and tok.second != "TERMINAL")
						{
							q.push(tok.first);
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
