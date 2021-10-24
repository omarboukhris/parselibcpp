#pragma once

#include <parselib/datastructure/common_types.h>

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
	~AbsNode () ;
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

	//
	// Constructors
	//

	Tree() ;
	Tree(AbsNode* node) ;

	//
	// Helpers and acccessors
	//

	/*!
	 * \brief merge inplace two trees into one
	 * \param tree tree to merge with *this
	 * \return current tree
	 */
	Tree * merge (Tree *tree) ;

	/*!
	 * \brief keyInTree check if the key is in the tree
	 * \param key to look for
	 * \return boolean
	 */
	size_t keyInTree (std::string key) ;

	virtual std::string getval () ;

	//
	// stream operators
	//

	friend std::ostream & operator<< (std::ostream& out, Tree* tree) ;
	friend std::ostream & operator<< (std::ostream& out, Tree tree) ;

private :
	/*!
	 * \brief dumps content of a tree into a string
	 * \param tree tree to unfold
	 * \param tab used for recursive tab stack tracking
	 * \return tree recursive dump of *tree parameter
	 */
	std::string dump (AbsNode *tree, std::string tab="") ;
};

/*!
 * \brief abstract node : generates the parse tree
 * unfold is a lazy eval
 */
class Node {

public :

	//
	// Constructors
	//

	Node () ;

	//
	// Helpers and accessors
	//

	/*!
	 * \brief iscompacted check if node is compacted
	 * check if there are / in token name
	 * \return boolean
	 */
	bool iscompacted () ;

	/*!
	 * \brief abstract, pure virtual
	 * unfolds the Node into a parse tree : Tree*
	 * \param parent  parent node
	 * \return Tree pointer
	 */
	virtual Tree* unfold (std::string parent="") = 0 ;

	/// \brief node type (terminal or non terminal)
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

