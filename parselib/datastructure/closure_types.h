#pragma once

#include <parselib/datastructure/common_types.h>

#include <utility>

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

        Item(const Item &it) = default;

        explicit Item(Rule r)
                : m_rule(std::move(r))
                , m_position(0)
        {
        }

        ~Item()= default;

        [[nodiscard]] size_t getPosition() const {
            return m_position;
        }

        Token read() {
            return (m_position < m_rule.size()) ?
                   m_rule.at(m_position) : Token();
        }

        void next() {
            ++m_position;
        }

        Token readNext() {
            return (m_position < m_rule.size()) ?
                   m_rule.at(m_position++) : Token();
        }

        void reset() {
            m_position = 0 ;
        }

        [[nodiscard]] bool done() const {
            return m_position >= m_rule.size() ;
        }

        Rule &getRule() {
            return m_rule;
        }

        void write_to (std::fstream &t_fstream) {

            try {
                // write position first
                t_fstream.write(reinterpret_cast<char*>(&m_position), sizeof(m_position));
                // then rule's size
                size_t ruleSize = m_rule.size();
                t_fstream.write(reinterpret_cast<char*>(&ruleSize), sizeof(ruleSize));

                for (const Token& tok: m_rule) {
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

            return it1.m_rule == it2.m_rule;
        }

        friend bool operator!=(const Item &it1, const Item &it2) {
            return not (it1 == it2);
        }

        friend bool operator==(const Item &it, const Rule &r) {
            return it.m_rule == r and it.done();
        }

        friend std::ostream& operator<<(std::ostream &out, const Item &item) {
//            out << item.m_position << " ";
            for (size_t i = 0 ; i < item.m_rule.size(); ++i) {
                if (i == item.m_position) {
                    out << "*";
                }
                out << item.m_rule.at(i) ;
            }
            if (item.m_rule.size() == item.m_position)
                out << "*";
            return out;
        }

    protected:
        Rule m_rule;
        size_t m_position;
    };

    class Closure {
    public:
        typedef std::vector<Item> Items;
        // struct instead of pair
        typedef std::pair<Token, std::string> Transition;
        typedef std::map<Token, std::string> Transitions ;

        typedef typename Items::iterator iterator;
        typedef typename Items::const_iterator const_iterator;

        Closure()
                : m_items()
                , m_transitions()
                , m_label()
        {}

        explicit Closure(std::string  label)
                : m_items()
                , m_transitions()
                , m_label(std::move(label))
        {}

        Closure(const Closure &cl) {
            m_items = cl.m_items;
            m_transitions = cl.m_transitions;
            m_label = cl.m_label;
        }

        Closure(Items  its, Transitions  tr, std::string  lab)
                : m_items(std::move(its))
                , m_transitions(std::move(tr))
                , m_label(std::move(lab))
        {}

        Closure(Items  its, std::string  lab)
                : m_items(std::move(its))
                , m_transitions()
                , m_label(std::move(lab))
        {}

        static Closure last_state(const std::string& name, Item &it) {
            Closure out(name);
            out.add_item(it);
            return out;
        }

        void write_to (std::fstream &t_fstream) {

            try {

                size_t closureSize = m_items.size();
                t_fstream.write(reinterpret_cast<char*>(&closureSize), sizeof(closureSize));
                for (Item i: m_items) {
                    i.write_to(t_fstream);
                }

                size_t transitionSize = m_transitions.size();
                t_fstream.write(reinterpret_cast<char*>(&transitionSize), sizeof(transitionSize));
                for (const auto& transition: m_transitions) {
                    Token trans_token = transition.first;
                    std::string state = transition.second;
                    trans_token.write_to(t_fstream);
                    t_fstream.write(reinterpret_cast<char*>(state.size()), sizeof(state.size()));
                    t_fstream.write(state.c_str(), state.size());
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
                    Token trans_token;
                    trans_token.read_from(t_fstream);

                    long transSize;
                    t_fstream.read(reinterpret_cast<char*>(&transSize), sizeof(transSize));
                    char *transition_state = new char[transSize];
                    t_fstream.read(reinterpret_cast<char*>(transition_state), transSize);

                    add_transition(trans_token, std::string(transition_state));
                    delete[] transition_state;
                }

            } catch (std::exception &e) {

                std::cerr << "(closure) caught exception while reading file: "
                          << e.what() << std::endl ;
            }
        }

        inline iterator begin() noexcept { return m_items.begin(); }
        [[nodiscard]] inline const_iterator begin() const noexcept { return m_items.cbegin(); }
        inline iterator end() noexcept { return m_items.end(); }
        [[nodiscard]] inline const_iterator end() const noexcept { return m_items.cend(); }

        friend bool operator==(const Closure &c1, const Closure &c2) {
            // compare items size
            const Items &it1 = c1.m_items, &it2 = c2.m_items;
            return it1 == it2;
        }

        friend std::ostream & operator<< (std::ostream &out, const Closure &c) {
            std::stringstream ss;
            for (const auto &t_pair: c.m_transitions) {
                ss << c.m_label << " -> " << t_pair.second
                    << " [label=\"" << t_pair.first << "\"];" << std::endl;
            }

            out << ss.str() ;
            return out;
        }

        void add_item(Item &i) {
            m_items.push_back(i);
        }

        void add_transition(const Token& t_token, const std::string& t_state) {
            m_transitions.emplace(t_token, t_state);
        }

        [[nodiscard]] std::string label() const {
            return m_label;
        }

        [[nodiscard]] bool is_reduce_action() const {
            return m_transitions.empty(); // or isn't acceptance state
        }

        bool empty() {
            return not size();
        }

        size_t size() {
            return m_items.size();
        }

        [[nodiscard]] const Transitions &transitions() const {
            return m_transitions;
        }

    private:
        Items m_items;
        Transitions m_transitions;
        std::string m_label;
    };


    struct TableBuilder {
//        using FlatProductionMap = std::vector<std::pair<std::string, Rule>>;
        using FlatProductionMap = std::map<std::string, Rule>;
        FlatProductionMap flat_prod;

        explicit TableBuilder() = default;

        explicit TableBuilder(const ProductionRules &productionRules) : flat_prod() {
            for (const auto &rules: productionRules) {
                int c = 0;
                for (const Rule &rule: rules.second) {
                    std::stringstream ss ;
                    ss << rules.first << ":" << c++;
                    flat_prod.emplace(ss.str(), rule);
                }
            }
        }

        [[nodiscard]] StrVect get_reduction (const Closure &c) const {
            auto output = StrVect();
            for (const auto& rule: flat_prod) {
                bool found = std::any_of(c.begin(), c.end(), [&rule] (const Item &i) {
                    return (i == rule.second);
                });
                if (found) {
                    output.push_back(rule.first);
                }
            }
            if (output.empty())
                throw std::runtime_error("Did not find appropriate rule");
            return output;
        }

        [[nodiscard]] static std::string get_goto(const std::string &token_type, const Closure &c) {
            auto find_pred = [&](const auto & t) {
                return t.first == Token(token_type, Token::NonTerminal);
            };
            auto it = std::find_if(c.transitions().begin(), c.transitions().end(), find_pred);
            if (it != c.transitions().end()) {
                return it->second; // return label
            }
            return {}; // empty cell
        }

        [[nodiscard]] static std::string get_shift(const std::string &token_type, const Closure &c) {
            auto find_pred = [&](const Closure::Transition & t) {
                return t.first == Token(token_type, Token::Terminal);
            };
            auto it = std::find_if(c.transitions().begin(), c.transitions().end(), find_pred);
            if (it != c.transitions().end()) {
                return it->second;
            }
            return {}; // empty cell
        }
    };

    class Closures : public std::vector<Closure> {

    public:

    };

}
