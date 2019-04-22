#include <parselib/datastructure/parsetree.hpp>

namespace parselib {
namespace parsetree {

AbsNode::AbsNode () : type("tree") {
	tokens = std::vector<Token> () ;
} 

void AbsNode::push_back(AbsNode::Token tok) {
	tokens.push_back(tok) ;
}

AbsNode::Token AbsNode::back() {
	return tokens.back();
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


Leaf::Leaf(std::string s) {
	type = "leaf" ;
	val = s ;
}

std::string Leaf::getval() {
	return val ;
}


Tree::Tree() {
	tokens = AbsNode::TokenList () ;
}
Tree::Tree(AbsNode* node) {
	tokens = AbsNode::TokenList () ;
	for (Token tok : node->tokens) {
		tokens.push_back(tok);
	}
}

Tree* Tree::merge(Tree* tree) {
	for (Token leaf : *tree) {
		tokens.push_back(leaf);
	}
	return this ;
}

std::string Tree::getval() {
	return "" ;
}

std::ostream& operator<<(std::ostream& out, Tree* tree) {
	out << tree->display(tree) ;
	return out ;
}

std::string Tree::display(AbsNode* tree, std::string tab) {
	std::string ss = "" ;
	for (AbsNode::Token tok : tree->tokens) {
		if (tok.second->type == "leaf") {
			ss += tab + tok.first + ":" + tok.second->getval() + "\n";
		} else {
			ss += tab + tok.first + " = {\n" 
				+ display(tok.second, tab+"\t")  
				+ tab + "}\n" ;
		}
	}
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
			AbsNode::Token(nodetype, unit->unfold(nodetype))
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
		AbsNode::Token(
			val, 
			new Leaf(nodetype)
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
		AbsNode::Token tok = std::make_pair(nodetype, tree) ;
		Tree* treeout = new Tree() ;
		treeout->push_back(tok);
		return treeout ;
	}
}

}
}