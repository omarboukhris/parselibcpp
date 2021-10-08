#pragma once

#include <parselib/parsers/absparser.hpp>
#include <parselib/operations/naiveparsers.hpp>

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
	
	void readNext() {
		position++;
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
	
	friend bool operator!=(Item it1, Item it2) {
		if (it1.position != it2.position) {
			return true;
		}

		Rule r1 = it1.rule, r2 = it2.rule;
		if (r1.size() != r2.size()) {
			return true;
		}

		for (int i = 0; i < r1.size() ; i++) {
			if (r1.at(i) != r2.at(i)){
				return true;
			}
		}

		return false ;
	}

private:
	Rule rule;
	size_t position;
};

class Closure {
public:
	typedef std::vector<Item> Items;
	typedef std::map<std::string, Closure*> Transitions ; 
	
	Closure()
		: items()
		, transitions()
		, label("")
		, current_item_pos(0)
	{}

	Closure(const Closure &cl) {
		items = cl.items;
		transitions = cl.transitions;
		label = cl.label;
		current_item_pos = cl.current_item_pos;
	}

	Closure(Items its, Transitions tr, std::string lab)
		: items(its)
		, transitions(tr)
		, label(lab)
		, current_item_pos(0)
	{}

	Closure(Items its, std::string lab)
		: items(its)
		, transitions()
		, label(lab)
		, current_item_pos(0)
	{}

	void reset() {
		current_item_pos = 0;
	}

	Item &begin() {
		reset();
		return items.at(current_item_pos);
	}

	const Item &end() {
		return items.back();
	}

	Item &next() {
		current_item_pos++;
		Item &current_item = items.at(current_item_pos);
		return current_item;
	}

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

private:
	Items items;
	Transitions transitions;
	std::string label;
	size_t current_item_pos;
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

		std::vector<Closure> graph;

		Rule axiom_rule = production_rules["AXIOM"].at(0);
		Item axiom (axiom_rule);
		Closure i0 = make_closure(axiom);

		Closure clos = i0;

		for (Item current_item = clos.begin(); 
			current_item != clos.end(); 
			current_item = clos.next()) 
		{
			while(not current_item.done()) {
				current_item.readNext();

				Closure newest_clos = make_closure(current_item);
				if (std::find(graph.begin(), graph.end(), newest_clos) == graph.end()) {
					// add the new item to closures
					graph.push_back(newest_clos);
				}
			}
		}
	}

	Closure make_closure(Item current_item) {
		Closure output;
		return output;
	}
	
	void shift_reduce();

};

}

}
