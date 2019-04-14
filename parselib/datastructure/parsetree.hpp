#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>

namespace parselib {

namespace parsetree {

typedef boost::any Leaf ; //std::string or Tree ptr
typedef std::pair<std::string, Leaf > Token ;

class Tree : public std::vector<Token> {
public :
	Tree * merge (Tree *tree) {
		for (Token leaf : *tree) {
			push_back(leaf);
		}
		return this ;
	}
};

class Node {
public :
	Node () {
		nodetype = "" ;
	} ;
	bool iscompacted () {
		return (nodetype.find("/") != std::string::npos) ;
	}
	virtual Tree* unfold (std::string parent="") {} ;

	std::string nodetype ;
} ;

class UnitNode : public Node {
public :
	UnitNode (std::string nodetype, Node* unit) {
		this->nodetype = nodetype ;
		this->unit = unit ;
	}

	virtual Tree* unfold (std::string parent="") {
		if (iscompacted() || parent == nodetype) {
			return unit->unfold(nodetype) ;
		} else { //if parent != None :
			//insert token in a tree and return it
			Tree tree = Tree() ;
			tree.push_back (Token(nodetype, unit->unfold(nodetype))) ;
			return new Tree(tree) ;
		}
	}
private :
	Node* unit ;
} ;

class TokenNode : public Node {
public :
	TokenNode(std::string nodetype, std::string val) {
		this->nodetype = nodetype ;
		this->val = val ;
	}
	virtual Tree* unfold (std::string /*parent=""*/) {
		Tree tree = Tree() ;
		tree.push_back(
			Token(val, nodetype)
		);
		return new Tree(tree) ;
	}
	std::string val ;
} ;

class BinNode : public Node { 
public :
	BinNode(std::string nodetype, Node* left, Node* right) {
		this->left = left ; 
		this->right = right ; 
		this->nodetype = nodetype ;
	}
	virtual Tree* unfold (std::string parent="") {
		Tree* tree = left->unfold(nodetype) ;
		tree->merge(right->unfold(nodetype)) ;
		if (iscompacted() || parent == nodetype) { 
			return tree ;
		} else {
			Token tok = std::make_pair(nodetype, tree) ;
			tree->clear() ;
			tree->push_back(tok);
			return tree ;
		}
	}
	
private:
	Node* left ;
	Node* right ;
} ;

} //namespace parsetree

} //namespace parselib

