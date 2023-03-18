
#include <parselib/utils/io.hpp>
#include <utility>

#include "parsetree.hpp"

namespace parselib::parsetree {

Tree::Tree ()
	: m_type(NodeType::Branch)
	, m_tokens()
{}

Tree::Tree(std::string s)
	: m_type (NodeType::Leaf)
	, m_val (std::move(s))
{}

Tree::Tree (const TreePtr &node)
	: m_type(node->m_type)
	, m_tokens(node->m_tokens)
{}

Tree::Tree (Tree* node)
	: m_type(node->m_type)
	, m_tokens(node->m_tokens)
{
}

void Tree::push_back(const Tree::Token& tok) {
	m_tokens.push_back(tok) ;
}

void Tree::clear() {
	m_tokens.clear() ;
}

Tree::TokenList::iterator Tree::begin() {
	return m_tokens.begin() ;
}

Tree::TokenList::iterator Tree::end() {
	return m_tokens.end() ;
}

size_t Tree::size() {
	return m_tokens.size() ;
}

TreePtr Tree::merge(const TreePtr& tree) {
	for (auto& leaf : *tree) {
		m_tokens.push_back(leaf);
	}
	return std::make_shared<Tree>(this) ;
}

int Tree::keyInTree(const std::string& key) {
	int i = 0 ;
	for (auto& item : m_tokens) {
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

	if (m_type == NodeType::Leaf) {
		ss = m_val;
	}
	else { // NodeType::Branch

		for (Tree::Token &tok : m_tokens) {
			ss += tok.second->strUnfold() + " ";
		}
	}

	return ss;
}

Tree::TokenList &Tree::tokens() {
	return m_tokens;
}

Tree::NodeType Tree::type() {
	return m_type;
}

std::string Tree::val() {

	std::string output ;

	if (m_type == NodeType::Leaf){
		output = m_val;
	}
	else {
		output = "None";
	}
	return output;
}


std::ostream& operator<<(std::ostream& out, const TreePtr& tree) {
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
std::string Tree::dump(const TreePtr& tree, const std::string& tab) {
	std::string strval ;
	// count how many terminals displayed
	int count = 0;
	// count if we dump non terminals
	bool dump_nonterminal = false;

    std::stringstream ss;
	for (Tree::Token &tok : tree->m_tokens) {

		if (tok.second->m_type == NodeType::Leaf) {
            strval += tab + "(" + tok.first + ":" + tok.second->val() + ")\n";
			count++ ;
		}
		else {
			dump_nonterminal = true;
            ss << tab << tok.first << " = {" << std::endl
                << dump(tok.second, tab+"\t")
                << tab << "}" << std::endl;
            strval += ss.str() ;
            strval.clear();
		}
	}

	// check, if this is for error display/handling
	// must be highest non recursive call
	// must not dump any non terminal token
	// must dump only one unique terminal token
	if (not dump_nonterminal and count == 1 and tab.empty())
		strval.pop_back();
	return strval ;
}


Node::Node() {
	nodetype = "" ;
}
bool Node::iscompacted() const {
	return (nodetype.find('/') != std::string::npos) ;
}


UnitNode::UnitNode(UnitNode *other) {
	this->nodetype = other->nodetype ;
	this->unit = other->unit ;
}

UnitNode::UnitNode(const std::string &nodetype, const NodePtr &unit) {
	this->nodetype = nodetype ;
	this->unit = unit ;
}

TreePtr UnitNode::unfold(const std::string &parent){
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

TokenNode::TokenNode(const std::string &nodetype, const std::string &val) {
	this->val = val ;
	this->nodetype = nodetype ;
}

TreePtr TokenNode::unfold(const std::string &) {

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

BinNode::BinNode(const std::string &nodetype, const NodePtr &left, const NodePtr &right) {
	this->left = left ;
	this->right = right ;
	this->nodetype = nodetype ;
}

TreePtr BinNode::unfold(const std::string &parent) {
	TreePtr tree = left->unfold(nodetype) ;
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
