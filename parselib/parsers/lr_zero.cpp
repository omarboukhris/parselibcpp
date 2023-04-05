
#include <string>

#include "lr_zero.hpp"

namespace parselib::parsers {

LR_zero::LR_zero (const Grammar& grammar) : AbstractParser(grammar), tokens(grammar.tokens) {
    build_graph();
    build_table();
}

void LR_zero::build_table() {

    // make action table
    TableBuilder tableBuilder(production_rules);
    for (const Closure &item: m_graph) {
        std::string item_name = item.label();
        m_action[item_name] = {};
        if (item.is_reduce_action()) {
            Cell c = Cell::reduce(tableBuilder.get_reduction(item));

            // add empty(epsilon) and $
            for (Token &tok: tokens) {
                m_action[item_name][tok.type()] = c;
            }
        }
        else {
            for (Token &tok: tokens) {
                m_action[item_name][tok.type()] = Cell::shift(TableBuilder::get_shift(tok.type(), item));
            }
        }
    }

    // make goto table
    for (const Closure &item: m_graph) {
        std::string item_name = item.label();
        for (const auto &rules: production_rules) {
            std::string label = rules.first;
            m_goto[item_name][label] = Cell::goTo(TableBuilder::get_goto(label, item));
        }
    }

    // need goto and action tables, still checking requirements
    // once tables built, shift reduce implem should just read from stack and apply rules from table
    // which data struct should tables be ?
    // maps probably for easy labeling or an unordered map (hash table) for quicker access
}

void LR_zero::build_graph(){

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
                current_item.next();
                Closure newest_clos = make_closure(j++, current_item);

                if (newest_clos.empty()) {
                    break ;
                }

                if (auto it = std::find(m_graph.begin(), m_graph.end(), newest_clos);
                        it == m_graph.end()
                        ) {
                    m_graph.push_back(newest_clos);
                    // add newest_clos to processing stack
                    stack.push_back(newest_clos);
                }
            }

            // once rule is fully read, make final state, add transitions
            Closure final_state = Closure::last_state(j++, current_item);
            if (auto it = std::find(m_graph.begin(), m_graph.end(), final_state);
                    it == m_graph.end()
                    ) {
                // add the new item to closures
                m_graph.push_back(final_state);
            }
        }
        stack.pop_back();
    }

    // get transitions
    for (Closure &c: m_graph) {
        for (Item i: c) {
            Token tok = i.readNext();
            for (const Closure& other_c: m_graph) {
                if (auto it = std::find(other_c.cbegin(), other_c.cend(), i);
                        it != other_c.cend() and not tok.value().empty()
                        ) {
                    c.add_transition(tok, other_c.label());
                }
            }
        }
    }
}

Closure LR_zero::make_closure(int id, Item &current_item) {

    Closure output(id);

    // this is where the magic happens
    Token token = current_item.read();
    if (token.key() == Token::Empty) {
        return output;
    }

    output.add_item(current_item);

    // second is term/non-term
    std::string rulename = token.value();

    // queue & processed list to avoid endless looping
    std::vector<std::string> q, processed;

    if (token.type() != Token::Terminal) {
        // process it only if not terminal
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
                    if (auto it = std::find(processed.begin(), processed.end(), tok.value());
                            it == processed.end() and tok.type() != Token::Terminal and tok.type() != Token::Empty)
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
