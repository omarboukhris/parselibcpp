#pragma once

#include <queue>

#include <parselib/datastructure/closure_types.h>
#include <parselib/parsers/naiveparsers.hpp>

#include "absparser.hpp"

namespace parselib::parsers {

    class Closures : public std::vector<Closure> {

    public:



    };

    class LR_zero : public AbstractParser {
    public :
        enum Action {
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

            static Cell reduce (long i) {
                return Cell {Action::reduce, std::to_string(i)};
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

        Frame membership (const TokenList /*word*/&) override {
            return {};
        }

        friend std::ostream & operator<< (std::ostream& out, const LR_zero& lrZero) {
            for (const auto & c: lrZero.m_graph)
                out << c << std::endl ;

            // print action and goto tables
            for (const auto &act: lrZero.m_action) {
                for (const auto &m: act.second) {
                    out << "state " << act.first << " reads " << m.first << " action " << m.second.to_string() << std::endl;
                }
            }
            out << std::endl;
            for (const auto &act: lrZero.m_goto) {
                for (const auto &m: act.second) {
                    if (not m.second.empty())
                        out << "state " << act.first << " reads " << m.first << " " << m.second.to_string() << std::endl;
                }
            }

            return out;
        }

    protected:

        void build_table() ;

        Closure make_closure(int id, Item &current_item);

        void build_graph();

        void shift_reduce();

    protected:
        TokenList tokens ;

        Closures m_graph;

        std::unordered_map<
                std::string,
                std::unordered_map<std::string, Cell>
        > m_goto; // non terminals
        std::unordered_map<
                std::string,
                std::unordered_map<std::string, Cell>
        > m_action; // terminals - shift/reduce
    };

}
