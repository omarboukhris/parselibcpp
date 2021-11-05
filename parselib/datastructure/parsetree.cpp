
#include <parselib/utils/io.hpp>

#include "parsetree.hpp"

namespace parselib {

namespace parsetree {

Tree::Tree ()
	: type(NodeType::Branch)
	, tokens()
{}

Tree::Tree (Tree* node)
	: type(node->type)
	, tokens(node->tokens)
{}


Tree::~Tree()
{}

void Tree::push_back(Tree::Token tok) {
	tokens.push_back(tok) ;
}

void Tree::clear() {
	tokens.clear() ;
}

Tree::TokenList::iterator Tree::begin() {
	return tokens.begin() ;
}

Tree::TokenList::iterator Tree::end() {
	return tokens.end() ;
}

size_t Tree::size() {
	return tokens.size() ;
}

Tree* Tree::merge(Tree* tree) {
	for (Token leaf : *tree) {
		tokens.push_back(leaf);
	}
	return this ;
}

int Tree::keyInTree(std::string key) {
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


std::string Tree::strUnfold() {

	std::string ss = "" ;

	for (Tree::Token tok : tokens) {
		ss += tok.second->getval() + " ";
	}

	return ss ;
}


Tree::Tree(std::string s) {
	type = NodeType::Leaf ;
	val = s ;
}

std::string Tree::getval() {

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


std::ostream& operator<<(std::ostream& out, Tree* tree) {
	out << tree->dump(tree) ;
	return out ;
}
std::ostream& operator<<(std::ostream& out, Tree tree) {
	out << tree.dump(&tree) ;
	return out ;
}

/*!
 * \brief Tree::dump dumps the content of a tree into a std::string
 */
std::string Tree::dump(Tree* tree, std::string tab) {
	std::string ss = "" ;
	// count how many terminals displayed
	int count = 0;
	// count if we dump non terminals
	bool dump_nonterminal = false;
	for (Tree::Token tok : tree->tokens) {
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

Tree* UnitNode::unfold(std::string parent){
	if (iscompacted() || parent == nodetype) {
		return unit->unfold(nodetype) ;
	} else { //if parent != None :
		//insert token in a tree and return it
		Tree *tree = new Tree() ;
		tree->push_back (
			Tree::Token(nodetype, unit->unfold(nodetype))
		) ;
		return tree ;
	}
}


TokenNode::TokenNode(std::string nodetype, std::string val) {
	this->val = val ;
	this->nodetype = nodetype ;
}

Tree* TokenNode::unfold(std::string) {
	Tree *tree = new Tree() ;
	tree->push_back(
		Tree::Token(
			nodetype, 
			new Tree(val)
		)
	) ;
	return tree ;
}


BinNode::BinNode(std::string nodetype, Node* left, Node* right) {
	this->left = left ; 
	this->right = right ; 
	this->nodetype = nodetype ;
}

Tree* BinNode::unfold(std::string parent) {
	Tree* tree = left->unfold(nodetype) ;
	tree->merge(right->unfold(nodetype)) ;
	if (iscompacted() || parent == nodetype) { 
		return tree ;
	} else {
		Tree::Token tok = std::make_pair(nodetype, tree) ;
		Tree* treeout = new Tree() ;
		treeout->push_back(tok);
		return treeout ;
	}
}

}
}
