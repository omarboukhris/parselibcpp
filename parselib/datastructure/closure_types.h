#ifndef CLOSURE_TYPES_H
#define CLOSURE_TYPES_H

#include <parselib/datastructure/common_types.h>

namespace parselib {

/*!
 * \brief The Item class serves as a container for rules
 * composing a closure in a LR parsing context
 */
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
	{
	}

	~Item(){}

	size_t getPosition() {
		return m_position;
	}

	Token readNext() {
		if (m_position < m_rule.size()){
			return m_rule.at(m_position++);
		}
		return Token();
	}

	void reset() {
		m_position = 0 ;
	}

	bool done() {
		return m_position > m_rule.size() ;
	}

	Rule &getRule(){
		return m_rule;
	}

	void write_to (std::fstream &t_fstream) {

		try {
			// write position first
			t_fstream.write(reinterpret_cast<char*>(&m_position), sizeof(m_position));
			// then rule's size
			size_t ruleSize = m_rule.size();
			t_fstream.write(reinterpret_cast<char*>(&ruleSize), sizeof(ruleSize));

			for (Token tok: m_rule) {
				// then rule's tokens
				tok.write_to(t_fstream);
			}
		} catch (std::exception &e) {

			std::cerr << "(item) caught exception while writing file: "
					  << e.what() << std::endl ;
		}
	}

	void read_from (std::fstream &t_fstream) {

		try {
			// write position first
			t_fstream.read(reinterpret_cast<char*>(&m_position), sizeof(m_position));
			// then rule's size
			size_t ruleSize ;
			t_fstream.read(reinterpret_cast<char*>(&ruleSize), sizeof(ruleSize));

			m_rule.clear();
			for (size_t i = 0 ; i < ruleSize;  ++i) {
				// then rule's tokens
				Token tok ;
				tok.read_from(t_fstream);
				m_rule.push_back(tok);
			}
		} catch (std::exception &e) {

			std::cerr << "(item) caught exception while reading file: "
					  << e.what() << std::endl ;
		}
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

protected:
	Rule m_rule;
	size_t m_position;
};

class Closure {
public:
	typedef std::vector<Item> Items;
	typedef std::set<std::string> Transitions ;

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

	void write_to (std::fstream &t_fstream) {

		try {

			size_t closureSize = m_items.size();
			t_fstream.write(reinterpret_cast<char*>(&closureSize), sizeof(closureSize));
			for (Item i: m_items) {
				i.write_to(t_fstream);
			}

			size_t transitionSize = m_transitions.size();
			t_fstream.write(reinterpret_cast<char*>(&transitionSize), sizeof(transitionSize));
			for (std::string transition: m_transitions) {
				t_fstream.write(reinterpret_cast<char*>(transition.size()), sizeof(transition.size()));
				t_fstream.write(transition.c_str(), transition.size());
			}

		} catch (std::exception &e) {

			std::cerr << "(closure) caught exception while writing file: "
					  << e.what() << std::endl ;
		}
	}

	void read_from (std::fstream &t_fstream) {

		try {

			size_t closureSize;
			t_fstream.read(reinterpret_cast<char*>(&closureSize), sizeof(closureSize));
			for (size_t i = 0 ; i < closureSize; ++i) {
				Item item;
				item.read_from(t_fstream);
				m_items.push_back(item);
			}

			size_t transitionSize;
			t_fstream.read(reinterpret_cast<char*>(&transitionSize), sizeof(transitionSize));
			m_transitions.clear();
			for (size_t i = 0 ; i < transitionSize; ++i) {
				size_t transSize;
				t_fstream.read(reinterpret_cast<char*>(&transSize), sizeof(transSize));

				char *transtxt = new char[transSize];
				t_fstream.write(transtxt, transSize);
				m_transitions.emplace(transtxt);
				delete transtxt;
			}

		} catch (std::exception &e) {

			std::cerr << "(closure) caught exception while reading file: "
					  << e.what() << std::endl ;
		}
	}

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

	void add_item(Item &i) {
		m_items.push_back(i);
	}

	void add_transition(std::string t_state) {
		m_transitions.emplace(t_state);
	}

	std::string label() {
		return m_label;
	}

	bool empty() {
		return not size();
	}

	size_t size() {
		return m_items.size();
	}

private:
	Items m_items;
	Transitions m_transitions;
	std::string m_label;
};

}


#endif // CLOSURE_TYPES_H