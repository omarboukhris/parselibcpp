#pragma once

#include <queue>

#include <parselib/datastructure/closure_types.h>
#include <parselib/parsers/seqparsers.hpp>

#include "absparser.hpp"

namespace parselib::parsers {

    class LR_zero : public AbstractParser {
    public :
        enum Action {
            none,
            shift,
            reduce,
            goTo,
            accepted,
        };
        struct Cell {
            Action action;
            std::string item;

            [[nodiscard]] std::string action_to_string() const {
                switch (action) {
                    case Action::shift:
                        return "shift";
                    case Action::reduce:
                        return "reduce";
                    case Action::goTo:
                        return "goto";
                    case Action::accepted:
                        return "accepted";
                    default:
                        return "None";
                }
            }

            static Cell shift (const std::string &ss) {
                return Cell {Action::shift, ss};
            }

            static Cell reduce (const std::string& ss) {
                return Cell {Action::reduce, ss};
            }

            static Cell goTo (const std::string &ss) {
                return Cell { Action::goTo, ss };
            }

            [[nodiscard]] std::string to_string() const {
                std::stringstream ss;
                ss << action_to_string() << " " << item ;
                return ss.str();
            }

            [[nodiscard]] inline bool empty() const {
                return item.empty();
            }
        };

        LR_zero () = default;
        explicit LR_zero (const Grammar& grammar) ;

        ~LR_zero () override = default;

        Frame membership (const TokenList &word) override ;

        friend std::ostream & operator<< (std::ostream& out, const LR_zero& lrZero) {
            out << "digraph G {" << std::endl;
            std::stringstream ss;
            ss << "States" << " [label=<<table>";
            for (const auto & c: lrZero.m_graph) {
                ss << "<tr><td>" << c.label() << "</td><td>";
                for (const auto& i: c)
                    ss << i << "<br/>";
                ss << "</td></tr>";
                out << c << std::endl ;
            }
            ss << "</table>>]";
            out << ss.str() << std::endl;

            out << "ActionGoToTable [label=<";
            // render header
            out << "<table><th><td></td>";
            for (const auto &tok: lrZero.tokens)
                out << "<td>" << tok.cvalue() << "</td>";
            out << "<td> $ </td>";
            for (const auto &tok: lrZero.production_rules)
                if (tok.first != Token::Axiom)
                    out << "<td>" << tok.first << "</td>";
            out << "</th>";

            // render table body
            for (const auto &act: lrZero.m_action) {
                out << "<tr><td>" << act.first << "</td>";
                for (const auto &tok: lrZero.tokens) {
                    auto multi_cell = act.second.equal_range(tok.cvalue());
                    out << "<td>";
                    for (auto cell_it = multi_cell.first; cell_it != multi_cell.second; ++cell_it) {
                        out << cell_it->second.to_string() << "<br/>";
                    }
                    out << "</td>";
                }
                auto eof_action_range = act.second.equal_range("$");
                out << "<td>";
                for (auto cell_it = eof_action_range.first; cell_it != eof_action_range.second; ++cell_it) {
                    out << cell_it->second.to_string() << "<br/>";
                }
                out << "</td>";
                for (const auto &tok: lrZero.production_rules) {
                    if (tok.first == Token::Axiom)
                        continue;
                    auto multi_cell = act.second.equal_range(tok.first);
                    out << "<td>";
                    for (auto cell_it = multi_cell.first; cell_it != multi_cell.second; ++cell_it) {
                        out << cell_it->second.to_string() << "<br/>";
                    }
                    out << "</td>";
                }
                out << "</tr>";
            }
            out << "</table>>];" << std::endl;
            out << "}" << std::endl;

//            int i = 0;
//            for (const auto &p: lrZero.flat_map) {
//                std::stringstream ss2;
//                for (const auto &r: p.second) {
//                    ss2 << r << " ";
//                }
//                out << i++ << " " << p.first << " : " << ss2.str() << std::endl;
//                ss.clear();
//            }

            return out;
        }

    protected:

        void export_graph(const std::string& path) const {
            std::ofstream fs;
            fs.open(path);
            if (fs.is_open())
                fs << *this << std::endl;
            fs.close();
            std::stringstream ss;
            ss << "dot -Tpng -O " << path ;
            std::system(ss.str().c_str());
        }

        void build_table() ;

        Closure make_closure(const std::string& name, Item &current_item);

        void build_graph();

    protected:
        TableBuilder tableBuilder;

        TokenList tokens ;

        Closures m_graph;

        std::unordered_map<
            std::string,
            std::unordered_multimap<std::string, Cell>
        > m_action; // terminals - shift/reduce // non terminals goto

        TableBuilder::FlatProductionMap flat_map;

        void reduce_fn(Frame &stack, StrVect &positions, const Cell &next_step);
    };

}
