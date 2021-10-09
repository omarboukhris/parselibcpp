#ifndef CLOSURE_TYPES_H
#define CLOSURE_TYPES_H

#include <parselib/datastructure/common_types.h>

namespace parselib {

class Item {
public:

	Item()
		: m_rule()
		, m_position(0)
	{}

	Item(const Item &it)
		: m_rule(it.m_rule)
		, m_position(it.m_position)
	{}

	Item(Rule r)
		: m_rule(r)
		, m_position(0)
	{}

	~Item(){}

	size_t getPosition() {
		return m_position;
	}

	Token readNext() {
		if (++m_position < m_rule.size()){
			return m_rule.at(m_position);
		}
		return Token();
	}

	void reset() {
		m_position = 0 ;
	}

	bool done() {
		return m_position >= m_rule.size();
	}

	Rule &getRule(){
		return m_rule;
	}

	friend bool operator==(const Item &it1, const Item &it2) {
		if (it1.m_position != it2.m_position) {
			return false;
		}

		const Rule &r1 = it1.m_rule, &r2 = it2.m_rule;
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
	Rule m_rule;
	size_t m_position;
};

class Closure {
public:
	typedef std::vector<Item> Items;
	typedef std::map<std::string, Closure*> Transitions ;

	typedef typename Items::iterator iterator;
	typedef typename Items::const_iterator const_iterator;

	Closure()
		: m_items()
		, m_transitions()
		, m_label("")
	{}

	Closure(const Closure &cl) {
		m_items = cl.m_items;
		m_transitions = cl.m_transitions;
		m_label = cl.m_label;
	}

	Closure(Items its, Transitions tr, std::string lab)
		: m_items(its)
		, m_transitions(tr)
		, m_label(lab)
	{}

	Closure(Items its, std::string lab)
		: m_items(its)
		, m_transitions()
		, m_label(lab)
	{}

	inline iterator begin() noexcept { return m_items.begin(); }
	inline const_iterator cbegin() const noexcept { return m_items.cbegin(); }
	inline iterator end() noexcept { return m_items.end(); }
	inline const_iterator cend() const noexcept { return m_items.cend(); }

	friend bool operator==(const Closure &c1, const Closure &c2) {
		// compare items size
		const Items &it1 = c1.m_items, &it2 = c2.m_items;
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
		m_items.push_back(i);
	}

private:
	Items m_items;
	Transitions m_transitions;
	std::string m_label;
};

}


#endif // CLOSURE_TYPES_H
