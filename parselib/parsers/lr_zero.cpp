
#include <string>

#include "lr_zero.hpp"

namespace parselib::parsers {

LR_zero::LR_zero (const Grammar& grammar) : AbstractParser(grammar), tokens(grammar.tokens) {
    // flat map
    tableBuilder = TableBuilder (production_rules);
    flat_map = tableBuilder.flat_prod;

    build_graph();
    build_table();
}

void LR_zero::build_table() {
    // THIS IS BUGGY! FIX IT PLS!
    // THX !

    // make action table
    for (const Closure &closure: m_graph) {
        std::string item_name = closure.label();
        m_action[item_name] = {};
        if (closure.is_reduce_action()) {
            for (auto &reduct : tableBuilder.get_reduction(closure)) {
                Cell c = Cell::reduce(reduct);
                for (Token &tok: tokens) {
                    m_action[item_name].emplace(tok.type(), c);
                }
                m_action[item_name].emplace("$", c);
            }
        }
        else {
            // reduction feels ok
            // goto / accepted / empty
            for (Token &tok: tokens) {
                m_action[item_name].emplace(
                    tok.type(), Cell::shift(TableBuilder::get_shift(tok.type(), closure))
                );
            }
        }
        if (item_name == "1")
            m_action[item_name].emplace("$", Cell {Action::accepted, ""});
    }

    // make goto table
    for (const Closure &item: m_graph) {
        std::string item_name = item.label();
        for (const auto &rules: production_rules) {
            std::string label = rules.first;
            m_action[item_name].emplace(label, Cell::goTo(TableBuilder::get_goto(label, item)));
        }
    }

}

void LR_zero::build_graph(){

    // use flat map here instead of prod rules?
    if (production_rules.empty()) {
        return ;
    }
    int j = 0;
    Rule axiom_rule = production_rules[Token::Axiom].at(0);
    Item axiom (axiom_rule);
    Closure i0 = make_closure(std::to_string(j++), axiom);

    std::vector<Closure> stack;

    m_graph.push_back(i0);
    stack.push_back(i0);

    while(not stack.empty()) {
        Closure clos(stack.back());

        for (Item& current_item : clos) {

            while(not current_item.done()) {
                current_item.next();
                Closure newest_clos = make_closure(std::to_string(j++), current_item);

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
            Closure final_state = Closure::last_state(std::to_string(j++), current_item);
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
                if (auto it = std::find(other_c.begin(), other_c.end(), i);
                        it != other_c.end() and not tok.value().empty()
                        ) {
                    c.add_transition(tok, other_c.label());
                }
            }
        }
    }
}

Closure LR_zero::make_closure(const std::string& name, Item &current_item) {

    Closure output(name);

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

    export_graph("/home/omar/projects/parselibcpp/data_rc/g.dot");

    TokenList word = w;
    word.emplace_back("End", "$");

    Frame stack, output;
    StrVect positions;
    std::string current_state("0");

    stack.push_back(TokenNode::make_token("Begin", "^")); // start token
    positions.push_back(current_state);

    auto word_it = word.begin();
    while (word_it != word.end()) {
        auto next_steps = m_action[current_state].equal_range(word_it->type());
        for(auto it = next_steps.first; it != next_steps.second; it++) {
            auto next_step = it->second;

            if (next_step.action == Action::shift) {
                stack.push_back(parsetree::TokenNode::make_token(word_it->type(), word_it->value()));
                positions.push_back(next_step.item);
                current_state = next_step.item;
                word_it++;
            } else if (next_step.action == Action::reduce) {
                reduce_fn(stack, positions, next_step);
                const auto &current_token = stack.back()->nodetype;
                current_state = positions.back();
                auto action_state = m_action[current_state];
                const auto &goto_pose_range = action_state.equal_range(current_token);
                for(auto goto_it = goto_pose_range.first; goto_it != goto_pose_range.second; ++goto_it) {
                    // wtf is this ??
                    const auto &goto_pose = goto_it->second;
                    positions.push_back(goto_pose.item);
                }
                current_state = positions.back();
            } else if (next_step.action == Action::accepted) {
                return {stack.back()};
            } else {
                throw std::runtime_error("Something bad happened, empty action/goto Cell");
            }
        }
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
}

void LR_zero::reduce_fn(Frame &stack, StrVect &positions, const LR_zero::Cell &next_step) {
    using namespace parsetree;

    auto range = flat_map.equal_range(next_step.item);
    for (auto it = range.first; it != range.second; ++it) {
        const auto& r = it->second;
        if (r.empty() or r.size() >= stack.size()) {
            continue;
        }

        auto knode = NodePtrVect();
        for (const auto& n: r) {
            const auto& token = stack.back();
            if (n.cvalue() != token->nodetype) {
                // should get better error tracking
                continue;
            }
            knode.push_back(token);
            positions.pop_back();
            stack.pop_back();
        }
        stack.push_back(KNode::make_knode(next_step.item, knode));
        return;
    }
    throw std::runtime_error("Rule empty or Rule size larger than stack content");
//    throw std::runtime_error("nodetype is different than rule type");
}

}
