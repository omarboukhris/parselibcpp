
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

            for (Token &tok: tokens) {
                m_action[item_name][tok.type()] = c;
            }
            m_action[item_name]["$"] = c;
        }
        else {
            for (Token &tok: tokens) {
                m_action[item_name][tok.type()] = Cell::shift(TableBuilder::get_shift(tok.type(), item));
            }
        }
        if (item_name == "1")
            m_action[item_name]["$"] = Cell {Action::accepted, ""};
    }

    // make goto table
    for (const Closure &item: m_graph) {
        std::string item_name = item.label();
        for (const auto &rules: production_rules) {
            std::string label = rules.first;
            m_action[item_name][label] = Cell::goTo(TableBuilder::get_goto(label, item));
        }
    }

    // flat map
    flat_map = tableBuilder.flat_prod;
}

void LR_zero::build_graph(){

    if (production_rules.empty()) {
        return ;
    }
    int j = 0;
    Rule axiom_rule = production_rules[Token::Axiom].at(0);
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

Frame LR_zero::membership(const TokenList &w) {
    using namespace parsetree;

    TokenList word = w;
    word.emplace_back("End", "$");

    Frame stack, output;
    StrList positions;
    std::string current_state("0");

    stack.push_back(TokenNode::make_token("Begin", "^")); // start token
    positions.push_back(current_state);

    auto it = word.begin();
    while (it != word.end()) {
        std::cout << "==========================" << std::endl;
        std::cout << *it << " " << it->type() << " " << current_state << std::endl;
        std::cout << "action " << m_action[current_state][it->type()].to_string() << std::endl ;
        Cell next_step = m_action[current_state][it->type()];

        if (next_step.action == Action::shift) {
            stack.push_back(parsetree::TokenNode::make_token(it->type(), it->value()));
            positions.push_back(next_step.item);
            current_state = next_step.item;
            it++;
        }
        else if (next_step.action == Action::reduce) {
            reduce_fn(stack, positions, next_step);
            const auto &current_token = stack.back()->nodetype;
            current_state = positions.back();
            const auto &goto_pose = m_action[current_state][current_token];
            positions.push_back(goto_pose.item);
            current_state = positions.back();
        }
        else if (next_step.action == Action::accepted) {
            return {stack.back()};
        }
        else {
            throw std::runtime_error("Something bad happened, empty action/goto Cell");
        }
        std::cout << "[" ;
        for (const auto &p: positions) std::cout << p << " ";
        std::cout << "]" << std::endl
            << "[ ";
        for (const auto& s: stack) std::cout << s->nodetype << " ";
        std::cout << " ]" << std::endl;

    }
    return stack;

    //this is it
    /*
     * Start at state 0
     * read from message (input) like an iterator (add $)
     * init read stack with ("^", 0)
     * look in action table for next state from it value
     * if state is shift
     *      stack token read with current state
     *      increment message iterator (only when shifting)
     * elif state is reduce
     *      create node
     *      get rule from index:flatprod in table builder
     *      generate tree node pop as much from the stack as needed (1 or 2)
     *      after reduction, check goto table for next state
     *      stack node in processed stack (r_state: popped tree nodes)
     * else
     *      throw exception with cool error tracing
     * loop
     */

    /*
     * Note : since grammar is normalized, no need for a vectorNode data struct
     * just use bin, unit and token:
     *      token for simple shifts
     *      unit for 1st order reduct
     *      bin for 2nd order reduct
     * output is handled through same pipeline
     */
}


    void LR_zero::reduce_fn(Frame &stack, StrList &positions, const LR_zero::Cell &next_step) {
        int idx = std::stoi(next_step.item);
        auto r = flat_map[idx];
        if (r.second.size() == 1) { // unit
            parsetree::NodePtr unit = stack.back(); stack.pop_back();
            positions.pop_back();
            stack.push_back(parsetree::UnitNode::make_unit(r.first, unit));
        }
        else if (r.second.size() == 2) { // bin
            parsetree::NodePtr left = stack.back(); stack.pop_back();
            parsetree::NodePtr right = stack.back(); stack.pop_back();
            positions.pop_back(); positions.pop_back();
            stack.push_back(parsetree::BinNode::make_bin(r.first, left, right));
        }
        else {
            throw std::runtime_error("Rule not bin or unit, normalize grammar!!");
        }
    }

}
