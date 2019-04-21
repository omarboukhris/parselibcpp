#pragma once

#include <parselib/datastructure/lexer.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class AbsNode {
public :
	AbsNode () : type("tree") {} 
	virtual std::string getval () {
		return "" ;
	}
	std::string type ;
} ;

// terminal node (a leaf in the tree)
class Leaf : public AbsNode {
public :
	Leaf (std::string s) ;
	virtual std::string getval () {
		return val ;
	}
	std::string val ;
} ;
// tree (still branching)
typedef std::pair<std::string, AbsNode*> Token ;
class Tree : public AbsNode {
public :
	Tree();
	Tree(AbsNode*) ;
	void push_back (Token tok) {
		tokens.push_back(tok) ;
	}
	Token back () {
		return tokens.back();
	}
	void clear () {
		tokens.clear() ;
	}
	std::vector<Token>::iterator begin () {
		return tokens.begin() ;
	}
	std::vector<Token>::iterator end () {
		return tokens.end() ;
	}
	size_t size () {
		return tokens.size() ;
	}
	Tree * merge (Tree *tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree* tree) ;
private :
	std::string display (Tree *tree, std::string tab) ;
	std::vector<Token> tokens ;
};

// abstract node : generates the parse tree 
// unfold is some kind of lazy eval
class Node {
public :
	Node () ;
	bool iscompacted () ;
	virtual Tree* unfold (std::string parent="") = 0 ;

	std::string nodetype ;
} ;

class UnitNode : public Node {
public :
	UnitNode (std::string nodetype, Node* unit) ;
	virtual Tree* unfold (std::string parent="") ;

private :
	Node* unit ;
} ;

class TokenNode : public Node {
public :
	TokenNode(std::string nodetype, std::string val) ;
	virtual Tree* unfold (std::string /*parent=""*/) ;

	std::string val ;
} ;

class BinNode : public Node { 
public :
	BinNode(std::string nodetype, Node* left, Node* right) ;
	virtual Tree* unfold (std::string parent="") ;
	
private:
	Node* left ;
	Node* right ;
} ;

} //namespace parsetree

} //namespace parselib

