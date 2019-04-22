#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class AbsNode {
public :
	typedef std::pair<std::string, AbsNode*> Token ;
	typedef std::vector<Token> TokenList ;

	
	AbsNode () : type("tree") {
		tokens = std::vector<Token> () ;
	} 
	virtual std::string getval () = 0 ;
	void push_back (Token tok) {
		tokens.push_back(tok) ;
	}
	Token back () {
		return tokens.back();
	}
	void clear () {
		tokens.clear() ;
	}
	TokenList::iterator begin () {
		return tokens.begin() ;
	}
	TokenList::iterator end () {
		return tokens.end() ;
	}
	size_t size () {
		return tokens.size() ;
	}
	std::string type ;
	TokenList tokens ;
} ;

// terminal node (a leaf in the tree)
class Leaf : public AbsNode {
public :
	Leaf (std::string s) {
		type = "leaf" ;
		val = s ;
	}
	virtual std::string getval () {
		return val ;
	}
	std::string val ;
} ;
// tree (still branching)
class Tree : public AbsNode {
public :
	Tree() {
		tokens = std::vector<AbsNode::Token> () ;
	}
	Tree(AbsNode* node) {
		tokens = std::vector<AbsNode::Token> () ;
		for (Token tok : node->tokens) {
			tokens.push_back(tok);
		}
	}
	Tree * merge (Tree *tree) {
		for (Token leaf : *tree) {
			tokens.push_back(leaf);
		}
		return this ;
	}
	virtual std::string getval () {
		return "" ;
	}
	friend std::ostream & operator<< (std::ostream& out, Tree* tree) {
		out << tree->display(tree) ;
		return out ;
	}
private :
	std::string display (AbsNode *tree, std::string tab="") {
		std::string ss = "" ;
		for (AbsNode::Token tok : tree->tokens) {
			if (tok.second->type == "leaf") {
				ss += tab + tok.first + ":" + tok.second->getval() + "\n";
			} else {
				ss += tab + tok.first + " = {\n" 
				   + display(tok.second, tab+"\t") + "\n"  + tab + "}\n" ;
			}
		}
		return ss ;
	}
};

// abstract node : generates the parse tree 
// unfold is some kind of lazy eval
class Node {
public :
	Node () {
		nodetype = "" ;
	}
	bool iscompacted ()  {
		return (nodetype.find("/") != std::string::npos) ;
	}
	virtual Tree* unfold (std::string parent="") = 0 ;

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
			Tree *tree = new Tree() ;
			tree->push_back (
				AbsNode::Token(nodetype, unit->unfold(nodetype))
			) ;
			return tree ;
		}
	}
private :
	Node* unit ;
} ;

class TokenNode : public Node {
public :
	TokenNode(std::string nodetype, std::string val) {
		this->val = val ;
		this->nodetype = nodetype ;
	}
	virtual Tree* unfold (std::string /*parent=""*/) {
		Tree *tree = new Tree() ;
		tree->push_back(
			AbsNode::Token(
				val, 
				new Leaf(nodetype)
			)
		) ;
		return tree ;
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
			AbsNode::Token tok = std::make_pair(nodetype, tree) ;
			Tree* treeout = new Tree() ;
			treeout->push_back(tok);
			return treeout ;
		}
	}

 private:
	Node* left ;
	Node* right ;
} ;

} //namespace parsetree

} //namespace parselib

