
#include <parselib/utils/io.hpp>

#include "parsetree.hpp"

namespace parselib {

namespace parsetree {

Tree::Tree ()
	: type(NodeType::Branch)
	, tokens()
{}

Tree::Tree(std::string s)
	: type (NodeType::Leaf)
	, val (s)
{
	tokens.push_back(
		Token(
			std::make_pair<std::string, TreePtr>(
				"TERMINAL",
				std::make_shared<Tree>(this))));
}

Tree::Tree (TreePtr node)
	: type(node->type)
	, tokens(node->tokens)
{}

Tree::Tree (Tree* node)
	: type(node->type)
	, tokens(node->tokens)
{
}

Tree::~Tree()
{}

void Tree::switch_type(NodeType t_type) {
	type = t_type;
}

void Tree::set_to_leaf() {
	switch_type(NodeType::Leaf);
}

void Tree::set_to_branch() {
	switch_type(NodeType::Branch);
}

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

Tree::TreePtr Tree::merge(Tree::TreePtr tree) {
	for (auto& leaf : *tree) {
		tokens.push_back(leaf);
	}
	return std::make_shared<Tree>(this) ;
}

int Tree::keyInTree(std::string key) {
	int i = 0 ;
	for (auto& item : tokens) {
		std::string tokentype = item.first ;
		if (tokentype == key) {
			return i ;
		}
		i++ ;
	}
	return -1 ;
}


std::string Tree::strUnfold() {
	std::string ss;

	if (type == NodeType::Leaf) {
		ss = val;
	}
	else { // NodeType::Branch

		for (Tree::Token &tok : tokens) {
			ss += tok.second.get()->strUnfold();
		}
	}

	return ss;
}


std::string Tree::getval() {

	std::string output ;

	if (this->type == NodeType::Leaf){
		output = val;
	}
	else {
		output = "None";
	}
	return output;
}


std::ostream& operator<<(std::ostream& out, Tree::TreePtr tree) {
	out << tree->dump(tree) ;
	return out ;
}
std::ostream& operator<<(std::ostream& out, Tree tree) {
	out << tree.dump(std::make_shared<Tree>(&tree)) ;
	return out ;
}

/*!
 * \brief Tree::dump dumps the content of a tree into a std::string
 */
std::string Tree::dump(Tree::TreePtr tree, std::string tab) {
	std::string ss = "" ;
	// count how many terminals displayed
	int count = 0;
	// count if we dump non terminals
	bool dump_nonterminal = false;

	for (Tree::Token &tok : tree->tokens) {

		if (tok.second->type == NodeType::Leaf) {
			ss += tab + "(" + tok.first + ":" + tok.second->getval() + ")\n";
			count++ ;
		}
		else {
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


UnitNode::UnitNode(UnitNode *other) {
	this->nodetype = other->nodetype ;
	this->unit = other->unit ;
}

UnitNode::UnitNode(std::string nodetype, NodePtr unit) {
	this->nodetype = nodetype ;
	this->unit = unit ;
}

Tree::TreePtr UnitNode::unfold(std::string parent){
	if (iscompacted() || parent == nodetype) {
		return unit->unfold(nodetype) ;
	} else { //if parent != None :
		//insert token in a tree and return it
		Tree tree ;
		tree.push_back (
			Tree::Token(nodetype, unit->unfold(nodetype))
		) ;
		return std::make_shared<Tree>(tree);
	}
}


TokenNode::TokenNode(TokenNode *other) {
	this->val = other->val ;
	this->nodetype = other->nodetype ;
}

TokenNode::TokenNode(std::string nodetype, std::string val) {
	this->val = val ;
	this->nodetype = nodetype ;
}

Tree::TreePtr TokenNode::unfold(std::string) {

	Tree::Token token (
		nodetype,
		std::make_shared<Tree>(Tree(val))
	);

	Tree tree;
	tree.push_back(token) ;

	return std::make_shared<Tree>(tree);
}


BinNode::BinNode(BinNode *other) {
	this->left = other->left ;
	this->right = other->right ;
	this->nodetype = other->nodetype ;
}

BinNode::BinNode(std::string nodetype, NodePtr left, NodePtr right) {
	this->left = left ;
	this->right = right ;
	this->nodetype = nodetype ;
}

Tree::TreePtr BinNode::unfold(std::string parent) {
	Tree::TreePtr tree = left->unfold(nodetype) ;
	tree->merge(right->unfold(nodetype)) ;

	if (iscompacted() || parent == nodetype) {
		return tree ;
	} else {
		Tree::Token tok = std::make_pair(nodetype, tree) ;
		Tree treeout ;
		treeout.push_back(tok);
		return std::make_shared<Tree>(treeout) ;
	}
}

}
}
