#pragma once

#include <queue>

#include <parselib/operations/naiveparsers.hpp>

#include "absparser.hpp"

namespace parselib {

namespace myparsers {

class Item {
public:

	Item() 
		: rule()
		, position(0)
	{}

	Item(const Item &it) {
		rule = it.rule ;
		position = it.position;
	}

	Item(Rule r)
		: rule(r)
		, position(0)
	{}
	
	~Item(){}
	
	size_t getPosition() {
		return position;
	}
	
	Token readNext() {
		if (++position < rule.size()){
			return rule.at(position);
		}
		return Token();
	}
	
	void reset() {
		position = 0 ;
	}
	
	bool done() {
		return position >= rule.size();
	}
	
	Rule &getRule(){
		return rule;
	}
	
	friend bool operator==(const Item &it1, const Item &it2) {
		if (it1.position != it2.position) {
			return false;
		}

		Rule r1 = it1.rule, r2 = it2.rule;
		if (r1.size() != r2.size()) {
			return false;
		}

		for (int i = 0; i < r1.size() ; i++) {
			if (r1.at(i) != r2.at(i)){
				return false;
			}
		}

		return true ;
	}

	friend bool operator!=(const Item &it1, const Item &it2) {
		return not (it1 == it2);
	}

private:
	Rule rule;
	size_t position;
};

class Closure {
public:
	typedef std::vector<Item> Items;
	typedef std::map<std::string, Closure*> Transitions ; 
	
	typedef typename Items::iterator iterator;
	typedef typename Items::const_iterator const_iterator;

	Closure()
		: items()
		, transitions()
		, label("")
	{}

	Closure(const Closure &cl) {
		items = cl.items;
		transitions = cl.transitions;
		label = cl.label;
	}

	Closure(Items its, Transitions tr, std::string lab)
		: items(its)
		, transitions(tr)
		, label(lab)
	{}

	Closure(Items its, std::string lab)
		: items(its)
		, transitions()
		, label(lab)
	{}

	inline iterator begin() noexcept { return items.begin(); }
	inline const_iterator cbegin() const noexcept { return items.cbegin(); }
	inline iterator end() noexcept { return items.end(); }
	inline const_iterator cend() const noexcept { return items.cend(); }

	friend bool operator==(const Closure &c1, const Closure &c2) {
		// compare items size 
		Items it1 = c1.items, it2 = c2.items;
		if (it1.size() != it2.size()) {
			return false;
		}
		
		for (size_t i = 0 ; i < it1.size() ; i++) {
			if (it1.at(i) != it2.at(i)) {
				return false;
			}
		}

		return true;
	}
	
	void push_back(Item &i) {
		items.push_back(i);
	}

private:
	Items items;
	Transitions transitions;
	std::string label;
};


class LR_zero : public AbstractParser {
public :	

	LR_zero () {}
	virtual ~LR_zero () {} 
	LR_zero (Grammar grammar) {
		production_rules = grammar.production_rules ;
		unitrelation = grammar.unitrelation ;
		build_table();
	}

	virtual Frame membership (TokenList word) {
		return Frame();
	}

private:

	void build_table() {

		if (production_rules.size() < 1) {
			return ;
		}

		Rule axiom_rule = production_rules["AXIOM"].at(0);
		Item axiom (axiom_rule);
		Closure i0 = make_closure(axiom);

		std::vector<Closure> graph;
		graph.push_back(i0);

		Closure clos = i0;

		for (Item current_item : clos) {

			while(not current_item.done()) {

				Closure newest_clos = make_closure(current_item);

				if (std::find(graph.begin(), graph.end(), newest_clos) == graph.end()) {
					// add the new item to closures
					graph.push_back(newest_clos);
				}
			}
		}
	}


	Closure make_closure(Item &current_item) {

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

	void shift_reduce();

};

}

}
