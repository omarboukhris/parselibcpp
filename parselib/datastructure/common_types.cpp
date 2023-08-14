
#include <parselib/datastructure/common_types.h>
#include <parselib/utils/io.hpp>

#include <string>

namespace parselib {

std::string GrammarStruct::getstr() {
    std::stringstream ss;

    for (const auto& item : production_rules) {
        std::string key = item.first ;
        Rules rules = item.second ;

        ss << "\nRULE " << key << " = [\n\t" ;

        StrVect rule_in_a_line = StrVect () ;

        for (const Rule& rule : rules) {
            StrVect ruletxt = StrVect () ;
            for (Token opr : rule) {
                ruletxt.push_back(opr.type()+"("+opr.value()+")");
            }
            std::string thisrule = utils::join(ruletxt, " ") ;
            rule_in_a_line.push_back(thisrule);
        }
        ss << utils::join(rule_in_a_line, "\n\t") + "]" ;
    }

    ss << "\n\nLABELS = [\n" ;
    for (const auto& item : labels) {
        std::string key = item.first ;
        LabelReplacement labmap = item.second ;
        ss << key << " {\n" ;
        for (const auto& lab : labmap) {
            ss << "\t" + lab.first + " : " + lab.second + "\n" ;
        }
        ss << "}\n" ;
    }
    ss << "]\n" ;

    ss << "STRUCT = [\n" ;
    for (const auto& item : keeper) {
        std::string key = item.first ;
        StrVect listkeep = item.second ;
        ss << "" << key << " {\n\t" << utils::join(listkeep, "\n\t") << "}\n" ;
    }
    ss << "\n]\n\n" ;

    for (const auto &node: strnodes) {
        ss << "STRNODE " << node.first << "= [\n" << utils::join(node.second, "") << "\n]\n\n";
    }

    for (Token tok : tokens) {
        std::string label = tok.type() ;
        std::string regx = tok.value() ;
        ss << "TOKEN " << label << " = regex('" << regx << "')" << std::endl ;
    }
    return ss.str();
}

} // namespace
