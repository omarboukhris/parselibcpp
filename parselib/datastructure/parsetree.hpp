#pragma once

#include <parselib/datastructure/lexer.hpp>

namespace parselib {

namespace parsetree {

// abstract class for composite pattern
class AbsNode {
public :
	typedef std::pair<std::string, AbsNode*> Token ;
	typedef std::vector<Token> TokenList ;

	std::string type ;
	TokenList tokens ;

	AbsNode () ;
	virtual std::string getval () = 0 ;
	std::string strUnfold() ;
	void push_back (Token tok) ;
	void clear () ;
	TokenList::iterator begin () ;
	TokenList::iterator end () ;
	size_t size () ;
	
} ;

// terminal node (a leaf in the tree)
class Leaf : public AbsNode {
public :
	Leaf (std::string s) ;
	virtual std::string getval () ;
	std::string val ;
} ;
// tree (still branching)
class Tree : public AbsNode {
public :
	Tree() ;
	Tree(AbsNode* node) ;
	Tree * merge (Tree *tree) ;
	size_t keyInTree (std::string key) ;
	Tree at(std::string key) ;
	virtual std::string getval () ;
	Tree operator[] (const char [] ) ;
	friend std::ostream & operator<< (std::ostream& out, Tree* tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree tree) ;
private :
	std::string dump (AbsNode *tree, std::string tab="") ;
};
typedef std::vector<Tree> TreeList ;

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

