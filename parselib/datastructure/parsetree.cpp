
#include <parselib/utils/io.hpp>

#include "parsetree.hpp"

namespace parselib {

namespace parsetree {

AbsNode::AbsNode ()
	: type(NodeType::Branch)
	, tokens()
{}

AbsNode::AbsNode (AbsNode* node)
	: type(node->type)
	, tokens(node->tokens)
{}


AbsNode::~AbsNode()
{}

void AbsNode::push_back(AbsNode::Token tok) {
	tokens.push_back(tok) ;
}

void AbsNode::clear() {
	tokens.clear() ;
}

AbsNode::TokenList::iterator AbsNode::begin() {
	return tokens.begin() ;
}

AbsNode::TokenList::iterator AbsNode::end() {
	return tokens.end() ;
}

size_t AbsNode::size() {
	return tokens.size() ;
}

AbsNode* AbsNode::merge(AbsNode* tree) {
	for (Token leaf : *tree) {
		tokens.push_back(leaf);
	}
	return this ;
}

int AbsNode::keyInTree(std::string key) {
	int i = 0 ;
	for (auto item : tokens) {
		std::string tokentype = item.first ;
		if (tokentype == key) {
			return i ;
		}
		i++ ;
	}
	return -1 ;
}


std::string AbsNode::strUnfold() {

	std::string ss = "" ;

	for (AbsNode::Token tok : tokens) {
		ss += tok.second->getval() + " ";
	}

	return ss ;
}


AbsNode::AbsNode(std::string s) {
	type = NodeType::Leaf ;
	val = s ;
}

std::string AbsNode::getval() {

	std::string output ;

	switch(type){
	case NodeType::Leaf:
		output = val;
		break;
	case NodeType::Branch:
		output = strUnfold();
		break;
	}

	return output;
}


std::ostream& operator<<(std::ostream& out, AbsNode* tree) {
	out << tree->dump(tree) ;
	return out ;
}
std::ostream& operator<<(std::ostream& out, AbsNode tree) {
	out << tree.dump(&tree) ;
	return out ;
}

/*!
 * \brief Tree::dump dumps the content of a tree into a std::string
 */
std::string AbsNode::dump(AbsNode* tree, std::string tab) {
	std::string ss = "" ;
	// count how many terminals displayed
	int count = 0;
	// count if we dump non terminals
	bool dump_nonterminal = false;
	for (AbsNode::Token tok : tree->tokens) {
		if (tok.second->type == NodeType::Leaf) {
			ss += tab + "(" + tok.first + ":" + tok.second->getval() + ")\n";
			count++ ;
		} else {
			dump_nonterminal = true;
			ss += tab + tok.first + " = {\n" 
				+ dump(tok.second, tab+"\t")
				+ tab + "}\n" ;
		}
	}

	// check, if this is for error display/handling
	// must be highest non recursive call
	// must not dump any non terminal token
	// must dump only one unique terminal token
	if (not dump_nonterminal and count == 1 and tab.size() == 0)
		ss.pop_back();
	return ss ;
}


Node::Node() {
	nodetype = "" ;
}
bool Node::iscompacted() {
	return (nodetype.find("/") != std::string::npos) ;
}


UnitNode::UnitNode(std::string nodetype, Node* unit) {
	this->nodetype = nodetype ;
	this->unit = unit ;
}

AbsNode* UnitNode::unfold(std::string parent){
	if (iscompacted() || parent == nodetype) {
		return unit->unfold(nodetype) ;
	} else { //if parent != None :
		//insert token in a tree and return it
		Tree *tree = new Tree() ;
		tree->push_back (
			AbsNode::Token(nodetype, unit->unfold(nodetype))
		) ;
		return tree ;
	}
}


TokenNode::TokenNode(std::string nodetype, std::string val) {
	this->val = val ;
	this->nodetype = nodetype ;
}

AbsNode* TokenNode::unfold(std::string) {
	AbsNode *tree = new AbsNode() ;
	tree->push_back(
		AbsNode::Token(
			nodetype, 
			new AbsNode(val)
		)
	) ;
	return tree ;
}


BinNode::BinNode(std::string nodetype, Node* left, Node* right) {
	this->left = left ; 
	this->right = right ; 
	this->nodetype = nodetype ;
}

AbsNode* BinNode::unfold(std::string parent) {
	AbsNode* tree = left->unfold(nodetype) ;
	tree->merge(right->unfold(nodetype)) ;
	if (iscompacted() || parent == nodetype) { 
		return tree ;
	} else {
		AbsNode::Token tok = std::make_pair(nodetype, tree) ;
		AbsNode* treeout = new Tree() ;
		treeout->push_back(tok);
		return treeout ;
	}
}

}
}
